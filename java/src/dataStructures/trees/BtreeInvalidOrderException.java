package dataStructures.trees;

class BtreeInvalidOrderExcetion extends RuntimeException {
    BtreeInvalidOrderExcetion(int order) {
        super("Order must be at least 3. You've provided order=" + order);
    }
}
