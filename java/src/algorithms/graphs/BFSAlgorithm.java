package algorithms.graphs;

import dataStructures.graphs.undirected.WeightedAdjacencyList;
import dataStructures.graphs.WeightedGraph;

import java.util.*;

public class BFSAlgorithm {

    // O(V+E) time complexity, O(V) space complexity
    public <V,W>  void bfs(WeightedGraph<V,W> graph,V source){
        StringBuilder path = new StringBuilder();
        Set<V> visited = new HashSet<>();

        visited.add(source);
        Queue<V> queue = new LinkedList<>();
        queue.add(source);

        while (!queue.isEmpty()){
            V vertex = queue.poll();

            path.append(vertex);
            path.append(" -> ");

            for(var edge: graph.getEdges(vertex)){
                if(visited.contains(edge.getFirst())) continue;
                visited.add(edge.getFirst());
                queue.add(edge.getFirst());
            }
        }

        path.delete(path.length() -4,path.length());
        System.out.println(path);
    }

    public static void main(String[] args) {
        BFSAlgorithm bfsAlgorithm = new BFSAlgorithm();
        WeightedAdjacencyList<Integer,Integer> adj = new WeightedAdjacencyList<>();

        for (int i = 0; i < 5;++i) adj.addVertex(i);
        adj.putEdge(0,1,1);
        adj.putEdge(1,2,1);
        adj.putEdge(1,3,1);
        adj.putEdge(1,4,1);
        adj.putEdge(2,3,1);

        bfsAlgorithm.bfs(adj,2);
    }
}
