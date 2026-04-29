package tree;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;

public class Btree<K extends Comparable<K>, V> {
    private int MAX_CHILDREN;
    private int MIN_CHILDREN;
    private int _size = 0;
    private Node root;

    Btree(int order) {
        if (order < 3)
            throw new BtreeInvalidOrderExcetion(order);
        MAX_CHILDREN = order;
        MIN_CHILDREN = Math.ceilDiv(order, 2);
    }

    public V search(K key) {
        if (_size == 0)
            return null;

        return _search(key, root);
    }

    public void insert(K key, V value) {
        if (_size == 0) {
            root = new Node(true);
            root.entries.add(new Entry(key, value));
            _size++;
            return;
        }

        _insert(key, value, null, 0);
    }

    public void remove(K key) {
        if (_size == 0)
            return;

        _remove(key,null,0);
    }

    public int size() {
        return _size;
    }

    private class Entry {
        public K key;
        public V value;

        public Entry(K key, V value) {
            this.key = key;
            this.value = value;
        }

        @Override
        public String toString() {
            StringBuilder builder = new StringBuilder();
            builder.append("[");
            builder.append(key);
            builder.append(": ");
            builder.append(value);
            builder.append("]");
            return builder.toString();
        }
    }

    private class Node {
        ArrayList<Entry> entries = new ArrayList<>();
        ArrayList<Node> children = new ArrayList<>();
        boolean leaf = false;

        Node(boolean leaf) {
            this.leaf = leaf;
        }
    };

    private void _insert(K key, V value, Node parent, int childIndex) {
        Node child = parent == null ? root : parent.children.get(childIndex);

        int i = 0;
        for (; i < child.entries.size(); ++i) {
            var entry = child.entries.get(i);
            if (key.compareTo(entry.key) <= 0)
                break;
        }

        int compareRes = i < child.entries.size() ? key.compareTo(child.entries.get(i).key) : 1;
        if (compareRes == 0) {
            // update node
            child.entries.get(i).value = value;
        } else if (!child.leaf) {
            _insert(key, value, child, i);
        } else {
            child.entries.add(i, new Entry(key, value));
            _size++;
        }

        checkSplit(parent, childIndex);
    }

    // Called when child has MAX_CHILDREN keys
    private void splitChild(Node parent, int childIndex) {
        Node child;
        if (parent == null) {
            child = root;
            parent = new Node(false);
            root = parent;
            childIndex = 0;
            parent.children.add(child);
        } else {
            child = parent.children.get(childIndex);
        }

        int middle = MIN_CHILDREN - 1;

        parent.entries.add(childIndex, child.entries.get(middle));

        Node rightSibling = new Node(child.leaf);

        for (int i = middle + 1; i < child.entries.size(); ++i) {
            rightSibling.entries.add(child.entries.get(i));
        }

        for (int i = child.entries.size() - 1; i >= middle; i--) {
            child.entries.remove(i);
        }

        parent.children.add(childIndex + 1, rightSibling);

        if (!child.leaf) {

            for (int i = middle + 1; i < child.children.size(); ++i) {
                rightSibling.children.add(child.children.get(i));
            }

            for (int i = child.children.size() - 1; i >= middle + 1; i--) {
                child.children.remove(i);
            }

        }

    }

    private void checkSplit(Node parent, int childIndex) {
        boolean shouldSplit = parent == null
                ? root.entries.size() == MAX_CHILDREN
                : parent.children.get(childIndex).entries.size() == MAX_CHILDREN;

        if (shouldSplit)
            splitChild(parent, childIndex);
    }

    private V _search(K key, Node node) {
        int i = 0;
        for (; i < node.entries.size(); ++i) {
            var entry = node.entries.get(i);
            if (key.compareTo(entry.key) <= 0)
                break;
        }

        int compareRes = i < node.entries.size() ? key.compareTo(node.entries.get(i).key) : 1;
        if (compareRes == 0)
            return node.entries.get(i).value;
        else if (node.leaf)
            return null;
        else
            return _search(key, node.children.get(i));
    }

    private void _remove(K key, Node parent, int childIndex) {
        Node child = parent == null ? root : parent.children.get(childIndex);

        int i = 0;
        for (; i < child.entries.size(); ++i) {
            var entry = child.entries.get(i);
            if (key.compareTo(entry.key) <= 0)
                break;
        }

        int compareRes = i < child.entries.size() ? key.compareTo(child.entries.get(i).key) : 1;
        if (compareRes == 0) {
            // key was found
            if (child.leaf) {
                deleteLeafNode(parent, childIndex, i);
            } else {
                deleteInternalNode(parent, childIndex, i);
            }
        } else if (!child.leaf) {
            _remove(key, child, i);
        }

        checkRebalance(parent,childIndex);
    }

    private void checkRebalance(Node parent, int childIndex){
        if(parent !=null && !hasMinKeys(parent.children.get(childIndex))){
            // rebalances everthing except root
            rebalanceNode(parent,childIndex);

            if (parent == root && parent.entries.isEmpty()) {
                // height of tree shrunk by 1
                Node newRoot = root.children.get(0);
                root.children.clear();
                root = newRoot;
            }
        }
    }

    private void rebalanceNode(Node parent, int childIndex){
        // check predecessor
        if (canBorrowFromLeftSibling(parent, childIndex)) {
            borrowLeftSiblingKey(parent, childIndex);
        }
        // check successor
        else if (canBorrowFromRightSibling(parent, childIndex)) {
            borrowRightSiblingKey(parent, childIndex);
        }
        // merge children
        else {
            mergeChildren(parent, childIndex);
        }
    }

    private void deleteInternalNode(Node parent, int childIndex, int keyIndex) {
        Node child = parent == null ? root : parent.children.get(childIndex);

        child.entries.remove(keyIndex);

        _size--;

        // Rebalances every node up to predecessor itself
        Entry entry = getPredecessor(child.children.get(keyIndex));
        child.entries.add(keyIndex,entry);

        if(!hasMinKeys(child.children.get(keyIndex))){
            rebalanceNode(child,keyIndex);
        }
    }

    private void deleteLeafNode(Node parent, int childIndex, int keyIndex) {
        Node child = parent == null ? root : parent.children.get(childIndex);

        child.entries.remove(keyIndex);

        _size--;

        if (child == root && _size == 0) {
            root = null;
        }
    }

    private Entry getPredecessor(Node parent) {
        if(parent.leaf) return parent.entries.removeLast();

        Entry predecessor = parent.children.getLast().leaf ? parent.children.getLast().entries.removeLast() : getPredecessor(parent.children.getLast());

        if (hasMinKeys(parent.children.getLast())) return predecessor;

        if(canBorrowFromLeftSibling(parent,parent.children.size() -1)){
            borrowLeftSiblingKey(parent,parent.children.size() -1);
        }else{
            mergeChildren(parent,parent.children.size() -1);
        }

        return predecessor;
    }

    private boolean hasMinKeys(Node node){
        return node.entries.size() >= MIN_CHILDREN -1;
    }

    private boolean canLendKey(Node node){
        return node.entries.size() > MIN_CHILDREN - 1;
    }

    private boolean canBorrowFromLeftSibling(Node parent, int childIndex) {
        return parent != null && childIndex > 0
                && canLendKey(parent.children.get(childIndex - 1));
    }

    private boolean canBorrowFromRightSibling(Node parent, int childIndex) {
        return parent != null && childIndex < parent.children.size() - 1
                && canLendKey(parent.children.get(childIndex + 1));
    }

    private void borrowLeftSiblingKey(Node parent, int childIndex) {
        Node lender = parent.children.get(childIndex - 1);
        Node receiver = parent.children.get(childIndex);

        int entryIndex = Math.min(childIndex,parent.entries.size() -1);

        receiver.entries.add(0, parent.entries.get(entryIndex));
        parent.entries.set(entryIndex, lender.entries.getLast());
        lender.entries.removeLast();

        if (!lender.leaf) {
            receiver.children.addFirst(lender.children.getLast());
            lender.children.removeLast();
        }
    }

    private void borrowRightSiblingKey(Node parent, int childIndex) {
        Node lender = parent.children.get(childIndex + 1);
        Node receiver = parent.children.get(childIndex);

        receiver.entries.addLast(parent.entries.get(childIndex));
        parent.entries.set(childIndex, lender.entries.getFirst());
        lender.entries.removeFirst();

        if (!lender.leaf) {
            receiver.children.addLast(lender.children.getFirst());
            lender.children.removeFirst();
        }
    }

    // Only called if parent is not null
    private void mergeChildren(Node parent, int childIndex) {
        Node merged;
        Node other;

        // Node has a right sibling
        if (childIndex < parent.children.size() - 1) {
            merged = parent.children.get(childIndex);
            other = parent.children.get(childIndex + 1);

            merged.entries.add(parent.entries.get(childIndex));

            parent.entries.remove(childIndex);
            parent.children.remove(childIndex + 1);
        }
        // Node has a left sibling only
        else {
            merged = parent.children.get(childIndex - 1);
            other = parent.children.get(childIndex);

            merged.entries.add(parent.entries.get(childIndex - 1));

            parent.entries.remove(childIndex - 1);
            parent.children.remove(childIndex);
        }

        for (int i = 0; i < other.entries.size(); ++i) {
            merged.entries.add(other.entries.get(i));
        }

        if (!merged.leaf) {
            for (int i = 0; i < other.children.size(); ++i) {
                merged.children.add(other.children.get(i));
            }
        }
    }

    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        Queue<Node> q = new LinkedList<>();
        q.add(root);

        int level = 1;

        while (!q.isEmpty()) {
            builder.append(level);
            builder.append(": ");
            int size = q.size();
            for (int j = 0; j < size; j++) {
                Node cur = q.poll();
                for (int i = 0; i < cur.entries.size(); ++i) {
                    builder.append(cur.entries.get(i));
                    builder.append("-");
                }
                builder.delete(builder.length() - 1, builder.length());

                if (!cur.leaf) {
                    for (int i = 0; i < cur.children.size(); ++i) {
                        q.add(cur.children.get(i));
                    }
                }
            }
            level++;
            builder.append("\n");
        }

        builder.delete(builder.length() - 1, builder.length());

        return builder.toString();
    }

    public static void main(String[] args) {
        Btree<Integer, Integer> btree = new Btree<>(3);

        for (int i = 1; i <= 20; ++i) {
            btree.insert(i, i);
        }
        System.out.println(btree);
        System.out.println("\n");

        btree.remove(12);
        System.out.println(btree);
        System.out.println("\n");

        btree.remove(4);
        System.out.println(btree);
        System.out.println("\n");

        System.out.println(btree.size());
    }

}
