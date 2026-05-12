package heap;

import java.util.*;

public class Heap<E> {
    private List<E> heap;
    private Comparator<E> comp;

    public Heap(Comparator<E> comparator, int initialCapacity) {
        heap = new ArrayList<>(initialCapacity);
        comp = comparator;
    }

    public Heap(Comparator<E> comparator) {
        heap = new ArrayList<>();
        comp = comparator;
    }

    public int size() {
        return heap.size();
    }

    public void insert(E element) {
        heap.add(element);

        if (heap.size() == 1) return;

        bubbleUp(heap.size() - 1);
    }

    public List<E> sort(){
        if(heap.size() <= 1) return heap;

        ArrayList<E> copy = new ArrayList<>(heap);

        // Sort
        ArrayList<E> sorted = new ArrayList<>(heap.size());
        while (!heap.isEmpty()){
            sorted.add(this.pop());
        }

        heap = copy;
        return sorted;
    }

    public void heapify(E... elements){
        if(elements.length <= 1){
            heap.addAll(List.of(elements));
            return;
        }

        int startIndex = Math.floorDiv(elements.length,2) - 1;
        for(int i = startIndex; i >= 0; --i){
            bubbleDown(i);
        }
    }

    public E peek(){
        return heap.isEmpty() ? null : heap.getFirst();
    }

    public E pop() {
        if (heap.isEmpty())
            return null;
        if (heap.size() == 1)
            return heap.removeLast();

        swap(heap.size() - 1, 0);
        E deleted = heap.removeLast();
        bubbleDown(0);

        return deleted;
    }

    private void swap(int i, int j) {
        E tmp = heap.get(i);
        heap.set(i, heap.get(j));
        heap.set(j, tmp);
    }

    private void bubbleDown(int index) {
        int parentIndex = index;
        int leftChildIndex = index * 2 + 1;
        int rightChildIndex = index * 2 + 2;

        while (parentIndex < heap.size()) {
            int minChildIndex = parentIndex;

            // finds the index of the minimum value between current node, left child and right child
            if (leftChildIndex < heap.size() && comp.compare(heap.get(minChildIndex), heap.get(leftChildIndex)) > 0)
                minChildIndex = leftChildIndex;
            if (rightChildIndex < heap.size() && comp.compare(heap.get(minChildIndex), heap.get(rightChildIndex)) > 0)
                minChildIndex = rightChildIndex;

            if (minChildIndex != parentIndex) {
                swap(parentIndex, minChildIndex);
                parentIndex = minChildIndex;
                leftChildIndex = parentIndex * 2 + 1;
                rightChildIndex = parentIndex * 2 + 2;
            } else
                break;
        }
    }

    private void bubbleUp(int index) {
        int childIndex = index;
        int parentIndex = Math.ceilDiv(index, 2) - 1;

        while (childIndex != 0) {
            int compareResult = comp.compare(heap.get(parentIndex), heap.get(childIndex));
            // if parent - child <= 0, it means that the parent should go before the child
            if (compareResult <= 0)
                break;
            swap(childIndex, parentIndex);
            childIndex = parentIndex;
            parentIndex = Math.ceilDiv(parentIndex, 2) - 1;
        }
    }

    public static void main(String[] args) {
        Heap<Integer> heap = new Heap<>((a, b) -> a - b);
        Heap<Integer> heap2 = new Heap<>((a, b) -> b - a);

        for (int i = 0; i < 10; ++i) {
            int randInt = (int) (Math.random() * 100 - 50);
            heap2.insert(randInt);
            heap.insert(randInt);
        }

        System.out.println(heap.sort());
        System.out.println(heap2.sort());

        while (heap2.size() > 0) {
            System.out.println(heap2.pop());
            System.out.println(heap.pop());
        }
    }

}
