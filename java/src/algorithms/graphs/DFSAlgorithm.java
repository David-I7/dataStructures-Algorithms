package algorithms.graphs;

import dataStructures.graphs.undirected.WeightedAdjacencyList;
import dataStructures.graphs.WeightedGraph;

import java.util.HashSet;
import java.util.Set;
import java.util.Stack;

public class DFSAlgorithm {

    // O(V + E) time complexity, O(V) space complexity
    public <V,W> void dfsR(WeightedGraph<V,W> graph, V source){
        StringBuilder path = new StringBuilder();
        Set<V> visited = new HashSet<>();
        _dfsR(graph,source,visited,path);
        path.delete(path.length() - 4,path.length());
        System.out.println(path.toString());
    }

    private <V,W> void _dfsR(WeightedGraph<V,W> graph, V cur, Set<V> visited, StringBuilder path){
        visited.add(cur);
        path.append(cur);
        path.append(" -> ");

        var edges = graph.edges(cur);

        for(var edge: edges){
            if(visited.contains(edge.getFirst())) continue;
            _dfsR(graph,edge.getFirst(),visited,path);
        }
    }

    // O(V + E) time complexity, O(V) space complexity
    public <V,W> void dfsItr(WeightedGraph<V,W> graph, V source){
        StringBuilder path = new StringBuilder();
        Set<V> visited = new HashSet<>();

        visited.add(source);
        Stack<V> stack = new Stack<>();
        stack.add(source);

        while (!stack.isEmpty()){
            V vertex = stack.pop();

            path.append(vertex);
            path.append(" -> ");

            for(var edge: graph.edges(vertex)){
                if(visited.contains(edge.getFirst())) continue;
                visited.add(edge.getFirst());
                stack.add(edge.getFirst());
            }
        }

        path.delete(path.length() -4,path.length());
        System.out.println(path);
    }

    public static void main(String[] args) {
        DFSAlgorithm dfsAlgorithm = new DFSAlgorithm();

        WeightedAdjacencyList<Integer,Integer> adj = new WeightedAdjacencyList<>();

        for (int i = 0; i < 5;++i) adj.addVertex(i);
        adj.putEdge(0,1,1);
        adj.putEdge(1,2,1);
        adj.putEdge(1,3,1);
        adj.putEdge(1,4,1);
        adj.putEdge(2,3,1);

        dfsAlgorithm.dfsR(adj,3);
        dfsAlgorithm.dfsItr(adj,3);
    }
}
