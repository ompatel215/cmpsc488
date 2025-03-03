import redis
import numpy as np

r = redis.Redis(host='localhost', port=6379, decode_responses=False)

def cosine_similarity(v1, v2):
    return np.dot(v1, v2) / (np.linalg.norm(v1) * np.linalg.norm(v2))

r.flushall()

charms = {
    "CHARM1": np.array([0.8, 0.3, 0.6, 0.9], dtype=np.float32),  # Large, blue-ish, complex pattern, shield shape
    "CHARM2": np.array([0.2, 0.9, 0.1, 0.3], dtype=np.float32),    # Small, bright white, simple pattern, orb shape
    "CHARM3": np.array([0.9, 0.7, 0.8, 0.7], dtype=np.float32),  # Very large, silvery, intricate pattern, animal shape
    "CHARM4": np.array([0.5, 0.4, 0.3, 0.4], dtype=np.float32),  # Medium, subtle glow, simple swish, curved shape
    "CHARM5": np.array([0.6, 0.8, 0.5, 0.5], dtype=np.float32)   # Medium-large, bright red, moderate pattern, beam shape
}

for charm_name, vector in charms.items():
    r.set(f"charm:{charm_name}", vector.tobytes())
    print(f"Added charm: {charm_name} with vector: {vector}")

def search_similar_charms(query_vector, top_k=3):
    results = []
    for charm_name, vector in charms.items():
        similarity = cosine_similarity(query_vector, vector)
        results.append((charm_name, similarity))
    
    results.sort(key=lambda x: x[1], reverse=True)
    return results[:top_k]

if __name__ == "__main__":
    # Query for a large, bright charm with moderate pattern complexity
    query_vector = np.array([0.7, 0.8, 0.5, 0.6], dtype=np.float32)
    print(f"\nQuery Vector: {query_vector}")

    results = search_similar_charms(query_vector)
    print("\nSimilarity Search Results:")
    for charm_name, similarity in results:
        print(f"Charm: {charm_name}, Similarity: {similarity:.4f}") 