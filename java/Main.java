
interface User{
    default String sayHello(){
        return "User2";
    };
}

class UserImpl implements User{

    public String sayHello(String s) {
        return "UsrImpl: " + s;
    }
}

public class Main {
    public static void main(String[] args) {
        UserImpl user = new UserImpl();
    }
}
