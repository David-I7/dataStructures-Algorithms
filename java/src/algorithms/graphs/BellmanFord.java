package algorithms.graphs;

import dataStructures.graphs.WeightedGraph;
import dataStructures.graphs.directed.WeightedEdgeList;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class BellmanFord {

    public <V> void perform(WeightedGraph<V,Integer> graph,V source){
        List<V> vertices = graph.getVertices();
        Map<V,Integer> result = new HashMap<>();

        vertices.forEach(v -> result.put(v,Integer.MAX_VALUE));

        result.put(source,0);

        for(int i = 0; i < vertices.size() - 1;++i){
            boolean hasChanged = false;
            for(var vertex: vertices){
                var curDistance = result.get(vertex);
                for (var edge : graph.getEdges(vertex)){
                    if(curDistance + edge.getSecond() < result.get(edge.getFirst())){
                        result.put(edge.getFirst(), curDistance + edge.getSecond());
                        hasChanged = true;
                    }
                }
            }
            if(!hasChanged) break;
        }


        for(var vertex: vertices){
            var curDistance = result.get(vertex);
            for (var edge : graph.getEdges(vertex)){
                if(curDistance + edge.getSecond() < result.get(edge.getFirst())){
                    result.put(edge.getFirst(), Integer.MIN_VALUE);
                }
            }
        }

        System.out.println(result);
    }

    public static void main(String[] args) {
        BellmanFord bf = new BellmanFord();

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

        bf.perform(graph,'a');
    }

}
