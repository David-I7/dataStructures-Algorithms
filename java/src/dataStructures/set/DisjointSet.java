package dataStructures.set;

public class DisjointSet {

    private int[] size;

    private int[] parent;

    private int numComponents;

    public DisjointSet(int capacity){
        parent = new int[capacity];
        size = new int[capacity];

        for(int i = 0 ; i < capacity; ++i){
            parent[i]= i;
            size[i] = 1;
        }
    }

    public boolean union(int a, int b){
        int pa = find(a);
        int pb = find(b);

        if(pa == pb) return false;

        if(size[pa] >= size[pb]){
            parent[pb] = pa;
            size[pa] += size[pb];
            size[pb] = 0;
        }else{
            parent[pa] = pb;
            size[pb] += size[pa];
            size[pa] = 0;
        }

        numComponents--;
        return true;
    }

    public int find(int p){
        if(parent[p] == p) return p;

        int child = p;

        while(parent[p] != p){
            p = parent[p];
        }

        while(parent[child] != p){
            int tmp = parent[child];
            parent[child] = p;
            child = tmp;
        }

        return p;
    }

    public int componentCount(){
        return numComponents;
    }

    public boolean connected(int p, int q) {
        return find(p) == find(q);
    }

}
