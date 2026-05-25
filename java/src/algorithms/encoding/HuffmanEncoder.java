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

        public boolean leaf(){
            return left == null && right == null;
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

        // Pre order traversal
        Stack<Node> stack = new Stack<>();

        Node current = huffmanTree;
        StringBuilder path = new StringBuilder();
        Node lastVisited = null;
        while (!stack.isEmpty() || current != null) {
            if (current != null) {
                stack.push(current);
                if (current.left != null) {
                    path.append('0');
                    current = current.left;
                } else if (current.right != null) {
                    path.append('1');
                    current = current.right;
                } else {
                    // It's a leaf node
                    huffmanCodesMap.put(current.character, path.toString());
                    current = null; // Force pop
                }
            } else {
                Node peekNode = stack.peek();
                // If right child exists and we're coming back from left child
                if (peekNode.right != null && lastVisited != peekNode.right) {
                    path.append('1');
                    current = peekNode.right;
                } else {
                    // We are done with this node, moving up
                    lastVisited = stack.pop();
                    if (!path.isEmpty()) {
                        path.deleteCharAt(path.length() - 1);
                    }
                }
            }
        }

        StringBuilder encoded = new StringBuilder();
        for (var ch: text.toCharArray()){
            encoded.append(huffmanCodesMap.get(ch));
        }

        return new EncodingResult(huffmanTree,encoded.toString());
    }

    public String decode(EncodingResult result){
        Node huffmanTree = result.huffmanTree;
        String encodedText = result.encoded;

        if(huffmanTree.leaf()){
            return String.valueOf(huffmanTree.character).repeat(result.encoded.length());
        }

        StringBuilder decoded = new StringBuilder();
        Node cur = huffmanTree;

        for(var bit: encodedText.toCharArray()){
            if (bit == '0') {
                cur = cur.left;
            } else {
                cur = cur.right;
            }

            if(cur.leaf()){
                decoded.append(cur.character);
                cur = huffmanTree;
            }
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
