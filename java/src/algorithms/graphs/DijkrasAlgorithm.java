package algorithms.graphs;

import dataStructures.graphs.WeightedGraph;
import dataStructures.graphs.directed.WeightedEdgeList;
import dataStructures.heap.Heap;
import dataStructures.util.Tuple;

import java.util.*;

public class DijkrasAlgorithm {
    public <V> Map<V,Integer> perform(WeightedGraph<V,Integer> graph, V start)  {
        Map<V,Integer> distances = new HashMap<>();
        var vertices = graph.vertices();

        for(var v: vertices){
            distances.put(v,Integer.MAX_VALUE);
        }

        distances.put(start,0);

        Heap<Tuple<V,Tuple<V,Integer>>> heap = new Heap<>((a, b) -> a.getSecond().getSecond().compareTo(b.getSecond().getSecond()));
        Set<V> visited = new HashSet<>();

        heap.insert(new Tuple<>(start,new Tuple<>(start,0)));
        while (heap.size() > 0){
            var top = heap.pop();
            V edgeTo = top.getSecond().getFirst();
            int distance = top.getSecond().getSecond();

            if(visited.contains(edgeTo)) continue;

            visited.add(edgeTo);
            for(var edge: graph.edges(edgeTo)){
                if(visited.contains(edge.getFirst())) continue;

                int curDistance = distance + edge.getSecond();
                int prevDistance = distances.get(edge.getFirst());
                if(curDistance < prevDistance){
                    distances.put(edge.getFirst(),curDistance);
                    heap.insert(new Tuple<>(edgeTo,new Tuple<>(edge.getFirst(),curDistance)));
                }
            }
        }

        return distances;
    }

    public static void main(String[] args) {
        DijkrasAlgorithm da = new DijkrasAlgorithm();

        WeightedGraph<Character,Integer> g= new WeightedEdgeList<>();
        g.putEdge('a','b',6);
        g.putEdge('a','c',2);
        g.putEdge('a','e',3);
        g.putEdge('b','d',1);
        g.putEdge('c','d',6);
        g.putEdge('c','e',2);
        g.putEdge('d','e',7);

        System.out.println(da.perform(g,'a'));
    }
}
