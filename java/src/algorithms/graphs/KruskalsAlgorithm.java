package algorithms.graphs;

import dataStructures.graphs.WeightedGraph;
import dataStructures.graphs.undirected.WeightedEdgeList;
import dataStructures.heap.Heap;
import dataStructures.set.DisjointSet;
import dataStructures.util.Tuple;

import java.util.*;

// Find the min spanning tree
public class KruskalsAlgorithm {

    // O(E log(E))
    public <V> void perform(WeightedGraph<V,Integer> graph){
        Heap<Tuple<V,Tuple<V,Integer>>> minHeap = new Heap<>((a,b) -> a.getSecond().getSecond() - b.getSecond().getSecond());
        List<V> vertices = graph.vertices();

        for(var v : vertices){
            for(var e: graph.edges(v)){
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

        // directed result
        System.out.println(minSpanningTree);
    }

    public <V> void performNonOptimal(WeightedGraph<V,Integer> graph){
        Heap<Tuple<V,Tuple<V,Integer>>> minHeap = new Heap<>((a,b) -> a.getSecond().getSecond() - b.getSecond().getSecond());
        List<V> vertices = graph.vertices();

        for(var v : vertices){
            for(var e: graph.edges(v)){
                minHeap.insert(new Tuple<>(v,e));
            }
        }

        Map<V,List<Tuple<V,Integer>>> minSpanningTree = new HashMap<>();

        while(minHeap.size() > 0){
            var edge = minHeap.pop();

            if(!createsCycle(minSpanningTree,edge.getFirst(),edge.getSecond().getFirst())){
                var fromList = minSpanningTree.computeIfAbsent(edge.getFirst(),(k) ->new ArrayList<>());
                var toList = minSpanningTree.computeIfAbsent(edge.getSecond().getFirst(),(k) ->new ArrayList<>());

                fromList.add(edge.getSecond());
                toList.add(new Tuple<>(edge.getFirst(), edge.getSecond().getSecond()));
            }
        }

        // undirected result
        System.out.println(minSpanningTree);
    }

    public <V> boolean createsCycle(Map<V,List<Tuple<V,Integer>>> tree, V a, V b){
        if(!tree.containsKey(a) || !tree.containsKey(b)) return false;

        Stack<V> stack = new Stack<>();
        Set<V> visited = new HashSet<>();

        stack.add(a);
        while (!stack.isEmpty()){
            V node = stack.pop();

            if(visited.contains(node)) continue;
            if(node.equals(b)) return true;

            visited.add(node);
            for(var edge: tree.get(node)){
                if(!visited.contains(edge.getFirst())){
                    stack.add(edge.getFirst());
                }
            }
        }

        return false;
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
        ka.performNonOptimal(graph);
    }
}
