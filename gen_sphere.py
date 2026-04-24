import math
from collections import defaultdict

def normalize(v):
    n = math.sqrt(v[0]**2 + v[1]**2 + v[2]**2)
    return (round(v[0]/n, 9), round(v[1]/n, 9), round(v[2]/n, 9))

t = (1 + math.sqrt(5)) / 2
verts = [
    (-1, t, 0), (1, t, 0), (-1, -t, 0), (1, -t, 0),
    (0, -1, t), (0, 1, t), (0, -1, -t), (0, 1, -t),
    (t, 0, -1), (t, 0, 1), (-t, 0, -1), (-t, 0, 1)
]

def get_mid(v1, v2):
    return ((verts[v1][0]+verts[v2][0])/2, (verts[v1][1]+verts[v2][1])/2, (verts[v1][2]+verts[v2][2])/2)

faces = [
    (0, 11, 5), (0, 5, 1), (0, 1, 7), (0, 7, 10), (0, 10, 11),
    (1, 5, 9), (5, 11, 4), (11, 10, 2), (10, 7, 6), (7, 1, 8),
    (3, 9, 4), (3, 4, 2), (3, 2, 6), (3, 6, 8), (3, 8, 9),
    (4, 9, 5), (2, 4, 11), (6, 2, 10), (8, 6, 7), (9, 8, 1)
]

edge_cache = {}
def add_midpoint(e):
    if e in edge_cache: return edge_cache[e]
    m = get_mid(e[0], e[1])
    verts.append(m)
    edge_cache[e] = len(verts) - 1
    return edge_cache[e]

for _ in range(2):
    new_faces = []
    for f in faces:
        a = add_midpoint((min(f[0], f[1]), max(f[0], f[1])))
        b = add_midpoint((min(f[1], f[2]), max(f[1], f[2])))
        c = add_midpoint((min(f[2], f[0]), max(f[2], f[0])))
        new_faces.extend([(f[0], a, c), (a, f[1], b), (c, b, f[2])])
    faces = new_faces

# Dedupe and normalize
seen = {}
unique_verts = []
face_indices = []

for v in verts:
    v = normalize(v)
    if v not in seen:
        seen[v] = len(unique_verts)
        unique_verts.append(v)
    face_indices.append(seen[v])

# Dedupe faces
unique_faces = set()
final_faces = []
for f in faces:
    idx0 = face_indices[f[0]]
    idx1 = face_indices[f[1]]
    idx2 = face_indices[f[2]]
    f2 = (min(idx0, idx1, idx2), idx0 + idx1 + idx2 - min(idx0, idx1, idx2) - max(idx0, idx1, idx2), max(idx0, idx1, idx2))
    if f2 not in unique_faces:
        unique_faces.add(f2)
        final_faces.append(f2)

with open('data/sphere_mesh.txt', 'w') as f:
    f.write(f'{len(unique_verts)} {len(final_faces)}\n')
    for x, y, z in unique_verts:
        f.write(f'{x} {y} {z}\n')
    for i0, i1, i2 in final_faces:
        f.write(f'{i0} {i1} {i2}\n')

print(f'{len(unique_verts)} vertices, {len(final_faces)} faces')