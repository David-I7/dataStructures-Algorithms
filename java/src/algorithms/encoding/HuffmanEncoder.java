package algorithms.encoding;

import dataStructures.heap.Heap;

import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Stack;

public class HuffmanEncoder {

    public class Node{
        int frequency;
        char character = 'x';
        Node left = null;
        Node right = null;

        public Node(int frequency,char character){
            this.frequency = frequency;
            this.character = character;
        }

        public Node(Node left, Node right){
            this.frequency = left.frequency + right.frequency;
            this.left = left;
            this.right = right;
        }
    }

    public class EncodingResult{
        private Node huffmanTree;
        private String encoded;

        public EncodingResult(Node huffmanTree, String encoded){
            this.huffmanTree = huffmanTree;
            this.encoded = encoded;
        }

        public String result() {
            return encoded;
        }

        @Override
        public String toString() {
            return "EncodingResult{" +
                    "encoded=" + encoded +
                    '}';
        }
    }

    public EncodingResult encode(String text){
        var frequencyMap = new HashMap<Character,Integer>();

        for(var c : text.toCharArray()){
            frequencyMap.put(c,frequencyMap.getOrDefault(c,0) + 1);
        }

        Heap<Node> heap = new Heap<>((a,b) -> a.frequency - b.frequency);

        for(var entry: frequencyMap.entrySet()){
            heap.insert(new Node(entry.getValue(),entry.getKey()));
        }

        while (heap.size() > 1){
            var leftChild = heap.pop();
            var rightChild = heap.pop();

            heap.insert(new Node(leftChild,rightChild));
        }

        Node huffmanTree = heap.pop();
        var huffmanCodesMap = new HashMap<Character,String>();

        // In order traversal
        Stack<Node> stack = new Stack<>();

        Node cur = huffmanTree;
        StringBuilder codes = new StringBuilder();
        while(!stack.isEmpty() || cur != null){

            while (cur != null){
               stack.add(cur);
               codes.append('0');
               cur = cur.left;
            }

            cur = stack.pop();
            codes.delete(codes.length()-1,codes.length());
            if(cur.left == null && cur.right == null) {
                huffmanCodesMap.put(cur.character, codes.toString());
            }
            if(cur.right != null){
                codes.append('1');
            }
            cur = cur.right;
        }

        StringBuilder encoded = new StringBuilder();
        for (var ch: text.toCharArray()){
            encoded.append(huffmanCodesMap.get(ch));
        }

        return new EncodingResult(huffmanTree,encoded.toString());
    }

    public String decode(EncodingResult encoded){
        Node huffmanTree = encoded.huffmanTree;
        String encodedText = encoded.encoded;

        StringBuilder decoded = new StringBuilder();
        Node cur = huffmanTree;
        for(var ch: encodedText.toCharArray()){
            if(cur.left == null && cur.right == null){
                decoded.append(cur.character);
                cur = huffmanTree;
            }
            if(ch == '0'){
                cur = cur.left;
            }else{
                cur = cur.right;
            }
        }
        if(cur.left == null && cur.right == null){
            decoded.append(cur.character);
            cur = huffmanTree;
        }

        return decoded.toString();
    }

    public static void main(String[] args) {
        HuffmanEncoder he = new HuffmanEncoder();

        String originalText = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.";
        //System.out.println(new String(originalText.getBytes(StandardCharsets.US_ASCII)));
        //String originalText = "AAABCDBe";
        System.out.println("Original text: " + originalText);
        HuffmanEncoder.EncodingResult encodedText = he.encode(originalText);
        System.out.println("Encoded text: " + encodedText.result());
        System.out.println("Decoded text: " + he.decode(encodedText));
    }
}
