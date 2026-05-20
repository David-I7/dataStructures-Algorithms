package dataStructures.graphs;

import dataStructures.util.Tuple;

import java.util.List;

public interface WeightedGraph<V,W> {
    boolean containsVertex(V vertex);

    boolean containsEdge(V source, V target);

    void addVertex(V vertex);

    void putEdge(V source, V target, W weight);

    void removeEdge(V source, V target);

    void removeVertex(V source);

    List<Tuple<V,W>> edges(V source);

    List<Tuple<V,Tuple<V,W>>> edges();

    List<V> vertices();

    int edgeCount();

    int verticesCount();
}

