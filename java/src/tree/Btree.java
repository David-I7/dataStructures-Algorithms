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

        if (checkSplit(parent, childIndex)) {
            splitChild(parent, childIndex);
        }
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

    private boolean checkSplit(Node parent, int childIndex) {
        boolean shouldSplit = parent == null
                ? root.entries.size() == MAX_CHILDREN
                : parent.children.get(childIndex).entries.size() == MAX_CHILDREN;

        return shouldSplit;
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
        Btree<Integer, Integer> btree = new Btree<>(4);

        for (int i = 0; i < 10; ++i) {
            btree.insert(i, i);
        }
        System.out.println(btree);
    }

}
