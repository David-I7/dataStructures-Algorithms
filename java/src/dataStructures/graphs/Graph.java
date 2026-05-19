package dataStructures.graphs;

import java.util.List;

public interface Graph<V> {
    boolean containsVertex(V vertex);

    boolean containsEdge(V source, V target);

    void addVertex(V vertex);

    void putEdge(V source, V target);

    void removeEdge(V source, V target);

    void removeVertex(V source);

    List<V> getEdges(V source);

    List<V> getVertices();

    int edgeCount();

    int verticesCount();
}
