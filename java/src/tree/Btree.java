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

    }

    private void deleteInternalNode(Node parent, int childIndex, int keyIndex) {
        Node child = parent == null ? root : parent.children.get(childIndex);

        child.entries.remove(keyIndex);

        int MIN_KEYS = MIN_CHILDREN - 1;

        if (child.entries.size() >= MIN_KEYS) {
            _size--;
            return;
        }

        if (child == root) {
            _size--;
            if (_size == 0)
                root = null;
            return;
        }

    }

    // Deletes key from leaf node
    private void deleteLeafNode(Node parent, int childIndex, int keyIndex) {
        Node child = parent == null ? root : parent.children.get(childIndex);

        child.entries.remove(keyIndex);

        int MIN_KEYS = MIN_CHILDREN - 1;

        if (child.entries.size() >= MIN_KEYS) {
            _size--;
            return;
        }

        if (child == root) {
            _size--;
            if (_size == 0)
                root = null;
            return;
        }

        // check predecessor
        if (canBorrowPredecessorKey(parent, childIndex)) {
            borrowPredecessorKey(parent, childIndex);
        }
        // check successor
        else if (canBorrowSuccessorKey(parent, childIndex)) {
            borrowSuccessorKey(parent, childIndex);
        }
        // merge children
        else {
            mergeChildren(parent, childIndex);
        }

    }

    private boolean canBorrowPredecessorKey(Node parent, int childIndex) {
        int MIN_KEYS = MIN_CHILDREN - 1;
        return parent != null && childIndex > 0
                && parent.children.get(childIndex - 1).entries.size() > MIN_KEYS;
    }

    private boolean canBorrowSuccessorKey(Node parent, int childIndex) {
        int MIN_KEYS = MIN_CHILDREN - 1;
        return parent != null && childIndex < parent.children.size() - 1
                && parent.children.get(childIndex + 1).entries.size() > MIN_KEYS;
    }

    private void borrowPredecessorKey(Node parent, int childIndex) {
        Node lender = parent.children.get(childIndex - 1);
        Node receiver = parent.children.get(childIndex);

        receiver.entries.add(0, parent.entries.get(childIndex));
        parent.entries.set(childIndex, lender.entries.getLast());
        lender.entries.removeLast();
    }

    private void borrowSuccessorKey(Node parent, int childIndex) {
        Node lender = parent.children.get(childIndex + 1);
        Node receiver = parent.children.get(childIndex);

        receiver.entries.addLast(parent.entries.get(childIndex));
        parent.entries.set(childIndex, lender.entries.getFirst());
        lender.entries.removeFirst();
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

        // if (parent == root) {
        // if (parent.entries.size() == 0) {
        // Node newRoot = root.children.get(0);
        // root.children.clear();
        // root = newRoot;
        // }
        // }
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
