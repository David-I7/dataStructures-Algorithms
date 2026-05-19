package algorithms.graphs;

import dataStructures.graphs.WeightedGraph;
import dataStructures.graphs.directed.WeightedEdgeList;
import dataStructures.heap.Heap;
import dataStructures.util.Tuple;

import java.util.*;

public class DijkrasAlgorithm {
    public <V> List<Tuple<V,Tuple<V,Integer>>> perform(WeightedGraph<V,Integer> graph, V start)  {
        List<Tuple<V,Tuple<V,Integer>>> result = new ArrayList<>();
        Heap<Tuple<V,Tuple<V,Integer>>> heap = new Heap<>((a, b) -> a.getSecond().getSecond().compareTo(b.getSecond().getSecond()));
        Set<V> visited = new HashSet<>();

        heap.insert(new Tuple<>(null,new Tuple<>(start,0)));
        while (heap.size() > 0){
            var top = heap.pop();
            if(visited.contains(top.getSecond().getFirst())) continue;

            result.add(top);
            V cur = top.getSecond().getFirst();
            visited.add(cur);
            for(var edge: graph.getEdges(cur)){
                if(visited.contains(edge.getFirst())) continue;
                heap.insert(new Tuple<>(cur,edge));
            }
        }

        return result;
    }

    public static void main(String[] args) {
        DijkrasAlgorithm da = new DijkrasAlgorithm();

        WeightedGraph<Character,Integer> g= new WeightedEdgeList<>();
        g.putEdge('a','b',4);
        g.putEdge('a','c',2);
        g.putEdge('b','c',3);
        g.putEdge('b','d',2);
        g.putEdge('b','e',3);
        g.putEdge('c','b',1);
        g.putEdge('c','d',4);
        g.putEdge('c','e',5);
        g.putEdge('e','d',1);

        System.out.println(da.perform(g,'a'));
    }
}
