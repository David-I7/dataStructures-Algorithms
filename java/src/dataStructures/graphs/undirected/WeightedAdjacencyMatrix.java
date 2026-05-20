package dataStructures.graphs.undirected;

import dataStructures.util.Tuple;

import java.util.*;

public class WeightedAdjacencyMatrix<V,W> implements UndirectedWeightedGraph<V,W> {

    private final List<List<Tuple<V,W>>> adjacencyMatrix = new ArrayList<>();
    private final Map<V,Integer> vertexToIndexMap = new HashMap<>();

    @Override
    // O(1)
    public boolean containsVertex(V vertex) {
        Integer index = vertexToIndexMap.get(vertex);
        return index != null;
    }

    @Override
    // O(1)
    public boolean containsEdge(V source, V target) {
        if(!containsVertex(source) || !containsVertex(target)) return false;

        Integer sourceIndex = vertexToIndexMap.get(source);
        Integer targetIndex = vertexToIndexMap.get(target);

        return adjacencyMatrix.get(sourceIndex).get(targetIndex) != null;
    }

    @Override
    // O(V)
    public void addVertex(V vertex) {
        if(containsVertex(vertex)) return;

        vertexToIndexMap.put(vertex,adjacencyMatrix.size());
        ArrayList<Tuple<V,W>> matrix= new ArrayList<>();
        fill(matrix,null,adjacencyMatrix.size());
        adjacencyMatrix.add(matrix);

        for(var row: adjacencyMatrix){
            row.add(null);
        }
    }

    @Override
    // O(1) if edges exist, otherwise O(V)
    public void putEdge(V source, V target, W weight) {
        if(!containsVertex(source)) addVertex(source);
        if(!containsVertex(target)) addVertex(target);

        Integer sourceIndex = vertexToIndexMap.get(source);
        Integer targetIndex = vertexToIndexMap.get(target);

        Tuple<V,W> oldSourceEdge = adjacencyMatrix.get(sourceIndex).get(targetIndex);
        if(oldSourceEdge == null){
            adjacencyMatrix.get(sourceIndex).set(targetIndex,new Tuple<>(target,weight));
        }else{
            oldSourceEdge.setSecond(weight);
        }

        Tuple<V,W> oldTargetEdge = adjacencyMatrix.get(targetIndex).get(sourceIndex);
        if(oldTargetEdge == null){
            adjacencyMatrix.get(targetIndex).set(sourceIndex,new Tuple<>(source,weight));
        }else{
            oldTargetEdge.setSecond(weight);
        }
    }

    @Override
    // O(1)
    public void removeEdge(V source, V target) {
        if(!containsVertex(source) || !containsVertex(target)) return;

        Integer sourceIndex = vertexToIndexMap.get(source);
        Integer targetIndex = vertexToIndexMap.get(target);

        adjacencyMatrix.get(sourceIndex).set(targetIndex,null);
        adjacencyMatrix.get(targetIndex).set(sourceIndex,null);
    }

    @Override
    // O(V)
    public void removeVertex(V source) {
        if(!containsVertex(source)) return;

        Integer sourceIndex = vertexToIndexMap.get(source);

        // Delete row
        adjacencyMatrix.remove((int)sourceIndex);

        // Delete reference from map
        vertexToIndexMap.remove(source);

        // Remove edge from remaining rows
        for(var row: adjacencyMatrix){
            swap(row,sourceIndex,row.size() -1);
            row.removeLast();
        }

        // Restore vertexToIndexMap
        for (var entry: vertexToIndexMap.entrySet()){
            if(entry.getValue() > sourceIndex){
                entry.setValue(entry.getValue() - 1);
            }
        }
    }

    @Override
    public List<Tuple<V, W>> edges(V source) {
        if(!containsVertex(source)) return null;
        return adjacencyMatrix.get(vertexToIndexMap.get(source));
    }

    @Override
    public List<Tuple<V, Tuple<V, W>>> edges() {
        return vertexToIndexMap.entrySet().stream()
            .map(entry-> adjacencyMatrix.get(entry.getValue()).stream().filter(Objects::nonNull).map(edge-> new Tuple<>(entry.getKey(),edge)).toList())
            .flatMap(List::stream)
            .toList();
    }

    @Override
    public List<V> vertices() {
        return vertexToIndexMap.keySet().stream().toList();
    }

    @Override
    public int edgeCount() {
        return adjacencyMatrix.stream()
                .reduce(0l,(acc,row) -> acc + row.stream().filter(Objects::nonNull).count(),Long::sum)
                .intValue();
    }

    @Override
    public int verticesCount() {
        return vertexToIndexMap.size();
    }

    private <T> void swap(List<T> list, int i, int j){
        var tmp = list.get(i);
        list.set(i,list.get(j));
        list.set(j,tmp);
    }

    private <T> void fill(ArrayList<T> list, T value, int capacity){
        for(int i = 0 ; i< capacity;++i){
            list.add(value);
        }
    }

    public static void main(String[] args) {
        WeightedAdjacencyMatrix<Integer,Integer> adj = new WeightedAdjacencyMatrix<>();

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
