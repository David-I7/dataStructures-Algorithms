package dataStructures.graphs.directed;

import dataStructures.graphs.WeightedGraph;
import dataStructures.util.Tuple;

import java.util.*;

public class WeightedAdjacencyList<V,W> implements DirectedWeightedGraph<V,W> {

    private Map<V, List<Tuple<V,W>>> adjacencyList = new HashMap<>();

    public WeightedAdjacencyList(){}

    @Override
    // O(1)
    public boolean containsVertex(V vertex){
        return adjacencyList.containsKey(vertex);
    }

    @Override
    // O(V) - V = number of vertices
    public boolean containsEdge(V source, V target){
        List<Tuple<V,W>> edges = adjacencyList.get(source);
        if(edges == null) return false;
        return containsEdge(edges,target);
    }

    @Override
    // O(1)
    public void addVertex(V vertex){
        adjacencyList.putIfAbsent(vertex,new ArrayList<>());
    }

    @Override
    // O(V) - V = number of vertices
    public void putEdge(V source, V target, W weight){
        if(!containsVertex(source)) addVertex(source);
        if(!containsVertex(target)) addVertex(target);

        List<Tuple<V,W>> sourceEdges = adjacencyList.get(source);

        Tuple<V,W> oldSourceEdge = findEdge(sourceEdges,target);
        if(oldSourceEdge == null){
            sourceEdges.add(new Tuple<>(target,weight));
        }else{
            oldSourceEdge.setSecond(weight);
        }
    }

    @Override
    // O(V) - V = number of vertices
    public void removeEdge(V source, V target){
        if(!containsVertex(source) || !containsVertex(target)) return;

        List<Tuple<V,W>> edges = adjacencyList.get(source);

        for(int i = 0; i < edges.size();++i){
            if(edges.get(i).getFirst().equals(target)){
                swap(edges,i,edges.size()-1);
                edges.removeLast();
                break;
            }
        }
    }

    @Override
    // O(V + E) - V = number of vertices, E = number of edges
    public void removeVertex(V source){
        List<Tuple<V,W>> edges = adjacencyList.remove(source);
        if(edges == null) return;

        for(var row: adjacencyList.values()){
            _removeEdge(row,source);
        }
    }

    @Override
    // O(1)
    public List<Tuple<V, W>> edges(V source) {
        return Collections.unmodifiableList(adjacencyList.get(source));
    }

    @Override
    public List<Tuple<V, Tuple<V, W>>> edges() {
        return adjacencyList.entrySet().stream()
                .map(entry-> entry.getValue().stream().map(edge -> new Tuple<>(entry.getKey(),edge)).toList())
                .flatMap(List::stream)
                .toList();
    }

    @Override
    public List<V> vertices() {
        return adjacencyList.keySet().stream().toList();
    }

    @Override
    public int edgeCount() {
        return adjacencyList.values().stream().reduce(0,(acc,b) -> acc + b.size(), Integer::sum);
    }

    @Override
    public int verticesCount() {
        return adjacencyList.size();
    }

    private <T> void swap(List<T> list, int i, int j){
        var tmp = list.get(i);
        list.set(i,list.get(j));
        list.set(j,tmp);
    }

    private boolean containsEdge(List<Tuple<V,W>> edges,V target){
        for(var edge: edges){
            if(edge.getFirst().equals(target)) return true;
        }
        return false;
    }

    private Tuple<V,W> findEdge(List<Tuple<V,W>> edges,V target){
        for(var edge: edges){
            if(edge.getFirst().equals(target)) return edge;
        }
        return null;
    }

    private void _removeEdge(List<Tuple<V,W>> edges, V target){
        for (int i = 0 ; i < edges.size();++i){
            if(edges.get(i).getFirst().equals(target)){
                swap(edges,i,edges.size() -1);
                edges.removeLast();
                break;
            }
        }
    }


    public static void main(String[] args) {
        WeightedAdjacencyList<Integer,Integer> adj = new WeightedAdjacencyList<>();

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
