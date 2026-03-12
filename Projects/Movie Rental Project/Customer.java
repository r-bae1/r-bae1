import java.util.LinkedList;

public class Customer {
    private final String _name;
    private final LinkedList<DiscountedRental> _rentals;
    int totalFrequentRenterPoints;
    private double totalAmountOwed;
    
    public Customer (String name) {
        _name = name;
        _rentals = new LinkedList<>();
    }

    LinkedList<DiscountedRental> getRentals() {
        return _rentals;
    }
   
    // Appends a rental to the list of rentals
    void addRental(DiscountedRental movieRental) {
        _rentals.add(movieRental);
        updateFRP(movieRental);
        totalAmountOwed += movieRental.getRentalPrice();
    }
    
    public String getName() {
        return _name;
    }

    public int getTotalFrequentRenterPoints() {
        return totalFrequentRenterPoints;
    }

    public double getTotalAmountOwed() {
        return totalAmountOwed;
    }

    private void updateFRP(DiscountedRental dr) {
        totalFrequentRenterPoints += dr.getFrequentRenterPoints();
    }
    
    // Returns the statement for a customer's rentals as a string
    public void presentStatementString() {
        PrintString print = new PrintString(this);
        print.printStatement();
    }
}
