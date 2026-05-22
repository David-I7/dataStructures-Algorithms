package algorithms.graphs;

import dataStructures.graphs.WeightedGraph;
import dataStructures.graphs.undirected.WeightedEdgeList;
import dataStructures.heap.Heap;
import dataStructures.util.Tuple;

import java.util.*;

public class PrimsAlgorithm {

    public <V> void perform(WeightedGraph<V,Integer> graph, V start){
        Heap<Tuple<V, Tuple<V,Integer>>> minHeap = new Heap<>((a,b) -> a.getSecond().getSecond() - b.getSecond().getSecond());
        Set<V> visited = new HashSet<>();

        Map<V, List<Tuple<V,Integer>>> minSpanningTree = new HashMap<>();
        minHeap.insert(new Tuple<>(null,new Tuple<>(start,0)));

        while (minHeap.size() > 0){
            var edge = minHeap.pop();
            var edgeTo = edge.getSecond().getFirst();
            var edgeFrom = edge.getFirst();

            if(visited.contains(edgeTo)) continue;

            if(!visited.isEmpty()) {
                var toList = minSpanningTree.computeIfAbsent(edgeTo, (k) -> new ArrayList<>());
                var fromList = minSpanningTree.computeIfAbsent(edgeFrom, (k) -> new ArrayList<>());

                fromList.add(edge.getSecond());
                toList.add(new Tuple<>(edgeFrom,edge.getSecond().getSecond()));
            }
            visited.add(edgeTo);

            for(var e: graph.edges(edgeTo)){
                if(visited.contains(e.getFirst())) continue;
                minHeap.insert(new Tuple<>(edgeTo,e));
            }

        }

        System.out.println(minSpanningTree);
    }

    public static void main(String[] args) {
        PrimsAlgorithm pa = new PrimsAlgorithm();
        WeightedGraph<Character,Integer> graph = new WeightedEdgeList<>();

        graph.putEdge('a','b',6);
        graph.putEdge('a','c',7);
        graph.putEdge('b','c',8);
        graph.putEdge('b','d',5);
        graph.putEdge('b','e',-4);
        graph.putEdge('c','d',-3);
        graph.putEdge('c','e',9);
        graph.putEdge('d','b',-2);
        graph.putEdge('e','a',2);
        graph.putEdge('e','d',7);

        pa.perform(graph,'a');
    }
}
