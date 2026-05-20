package dataStructures.graphs;

import dataStructures.util.Tuple;

import java.util.List;

public interface Graph<V> {
    boolean containsVertex(V vertex);

    boolean containsEdge(V source, V target);

    void addVertex(V vertex);

    void putEdge(V source, V target);

    void removeEdge(V source, V target);

    void removeVertex(V source);

    List<V> edges(V source);

    List<Tuple<V,V>> edges();

    List<V> vertices();

    int edgeCount();

    int verticesCount();
}
