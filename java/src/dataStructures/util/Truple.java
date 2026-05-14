package dataStructures.util;

import java.util.Objects;

public class Truple<A,B,C> {

    private A first;
    private B second;
    private C third;
    
    public Truple(A first, B second,C third){
        this.first = first;
        this.second = second;
        this.third = third;
    }

    public A getFirst() {
        return first;
    }

    public void setFirst(A first) {
        this.first = first;
    }

    public B getSecond() {
        return second;
    }

    public void setSecond(B second) {
        this.second = second;
    }

    public C getThird() {
        return third;
    }

    public void setThird(C third) {
        this.third = third;
    }

    @Override
    public boolean equals(Object object) {
        if (object == null || getClass() != object.getClass()) return false;
        Truple<?, ?, ?> truple = (Truple<?, ?, ?>) object;
        return Objects.equals(first, truple.first) && Objects.equals(second, truple.second) && Objects.equals(third, truple.third);
    }

    @Override
    public int hashCode() {
        return Objects.hash(first, second, third);
    }

    @Override
    public String toString() {
        return "(" +
                first +
                ", " +
                second +
                "," +
                third +
                ')';
    }
}
