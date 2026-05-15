package dataStructures.graphs.directed;

import dataStructures.util.Tuple;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class WeightedEdgeList<V,W> implements DirectedWeightedGraph<V,W> {

    private List<Tuple<V,Tuple<V,W>>> edgeList = new ArrayList<>();
    private Set<V> vertices = new HashSet<>();

    @Override
    // O(1)
    public boolean containsVertex(V vertex) {
        return vertices.contains(vertex);
    }

    @Override
    // O(V + E)
    public boolean containsEdge(V source, V target) {
        return edgeList.stream()
                .anyMatch((edge) -> isEdge(source,target,edge));
    }

    @Override
    // O(1)
    public void addVertex(V vertex) {
        vertices.add(vertex);
    }

    @Override
    // O(V + E)
    public void putEdge(V source, V target, W weight) {
        if(!vertices.contains(source)) addVertex(source);
        if(!vertices.contains(target)) addVertex(target);

        var prevEdge =
                edgeList.stream()
                        .filter(edge-> isEdge(source,target,edge))
                        .findFirst();

        if(prevEdge.isEmpty()){
            edgeList.add(new Tuple<>(source,new Tuple<>(target,weight)));
        }else{
            prevEdge.get().getSecond().setSecond(weight);
        }
    }

    @Override
    // O(V + E)
    public void removeEdge(V source, V target) {
        if(!vertices.contains(source) || !vertices.contains(target)) return;
        edgeList = edgeList.stream()
                .filter(edge -> !isEdge(source,target,edge))
                .toList();
    }

    @Override
    // O(V + E)
    public void removeVertex(V source) {
        vertices.remove(source);
        edgeList = edgeList.stream()
                .filter(edge -> !edge.getFirst().equals(source))
                .toList();
    }

    @Override
    // O(V + E)
    public List<Tuple<V, W>> getEdges(V source) {
        return edgeList.stream()
                .filter((edge) -> edge.getFirst().equals(source))
                .map(edge->edge.getSecond())
                .toList();
    }

    private boolean isEdge(V source, V target, Tuple<V,Tuple<V,W>> edge){
        return (edge.getFirst().equals(source) &&
                edge.getSecond().getFirst().equals(target));
    }

    public static void main(String[] args) {
        WeightedEdgeList<Integer,Integer> adj = new WeightedEdgeList<>();

        for (int i = 0; i < 5;++i) adj.addVertex(i);
        adj.putEdge(0,1,1);
        adj.putEdge(1,2,1);
        adj.putEdge(1,3,1);
        adj.putEdge(1,4,1);
        adj.putEdge(2,3,1);

        adj.removeEdge(1,2);
        adj.removeVertex(1);
    }
}
