package algorithms.graphs;

import dataStructures.graphs.WeightedGraph;
import dataStructures.graphs.directed.WeightedEdgeList;
import dataStructures.heap.Heap;
import dataStructures.set.DisjointSet;
import dataStructures.util.Tuple;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class KruskalsAlgorithm {

    public <V> void perform(WeightedGraph<V,Integer> graph){
        Heap<Tuple<V,Tuple<V,Integer>>> minHeap = new Heap<>((a,b) -> a.getSecond().getSecond() - b.getSecond().getSecond());
        List<V> vertices = graph.getVertices();

        for(var v : vertices){
            for(var e: graph.getEdges(v)){
                minHeap.insert(new Tuple<>(v,e));
            }
        }

        Map<V,Integer> verticesToIndex = new HashMap<>();
        for(int i = 0 ; i < vertices.size(); ++i){
            verticesToIndex.put(vertices.get(i),i);
        }

        Map<V,List<Tuple<V,Integer>>> minSpanningTree = new HashMap<>();
        DisjointSet disjointSet = new DisjointSet(vertices.size());
        while(minHeap.size() > 0){
            var edge = minHeap.pop();
            int indexFrom = verticesToIndex.get(edge.getFirst());
            int indexTo = verticesToIndex.get(edge.getSecond().getFirst());
            if(disjointSet.union(indexFrom,indexTo)){
                List<Tuple<V,Integer>> edges = minSpanningTree.computeIfAbsent(edge.getFirst(),(k)-> new ArrayList<>());
                edges.add(edge.getSecond());
            }
        }

        System.out.println(minSpanningTree);
    }

    public static void main(String[] args) {
        KruskalsAlgorithm ka = new KruskalsAlgorithm();
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

        ka.perform(graph);
    }
}
