//Calculates FRP depending on if it's a new release or regular.
public interface FRPStrategy {
    int frp(Rental r);
}

class FRPDefault implements FRPStrategy {
    @Override
    public int frp(Rental r) {
        return 1;
    }
}

class FRPNewRelease implements FRPStrategy {
    @Override
    public int frp(Rental r) {
        return r._daysRented > 1 ? 2 : 1;
    }
}
