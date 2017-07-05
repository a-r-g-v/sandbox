PARENT = {}

def MAKE_SET(v):
    PARENT.update({v : v})

def FIND_SET(u):
    if PARENT[u] == u:
        return u
    else:
        return FIND_SET(PARENT[u])

def UNION(u, v):
    u_root = FIND_SET(u) 
    v_root = FIND_SET(v)
    PARENT[u_root] = v_root

def SAME(u, v):
    return FIND_SET(u) == FIND_SET(v)



MAKE_SET(1)
MAKE_SET(2)
MAKE_SET(3)
UNION(1, 2)
assert SAME(1, 2) is True
assert SAME(1, 3) is False
