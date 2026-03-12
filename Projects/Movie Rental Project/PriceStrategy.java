public interface PriceStrategy {
    double getRentalPrice(Rental r);
}

// PriceStrategy interface defines the method to calculate rental price based on the rental type
class PriceRegular implements PriceStrategy {
    private final double BASE_RENTAL_AMOUNT = 2;
    private final int MIN_DAYS_RENTED = 2;
    private final double RENTAL_MULTIPLIER = 1.5;

    // Calculates the rental price for a regular movie based on the number of days rented
    // Regular movies are rented at a flat rate for the first two days, and then at a daily rate of $1.50
    @Override
    public double getRentalPrice(Rental r) {
        double total = BASE_RENTAL_AMOUNT;
        if (r._daysRented > MIN_DAYS_RENTED) {
            total += (r._daysRented - MIN_DAYS_RENTED) * RENTAL_MULTIPLIER;
        }
        return total;
    }
}

// NewRelease class implements the PriceStrategy interface for new release movies
class PriceNewRelease implements PriceStrategy {
    private final double RENTAL_MULTIPLIER = 3;

    @Override
    // Calculates the rental price for a new release movie based on the number of days rented
    public double getRentalPrice(Rental r) {
        return r._daysRented * RENTAL_MULTIPLIER;
    }
}

// Childrens class implements the PriceStrategy interface for children's movies
class PriceChildrens implements PriceStrategy {
    private final double BASE_RENTAL_AMOUNT = 1.5;
    private final int MIN_DAYS_RENTED = 3;
    private final double RENTAL_MULTIPLIER = 1.5;

    @Override
    // Calculates the rental price for a children's movie based on the number of days rented
    // Children's movies are rented at a flat rate for the first three days, and then at a daily rate of $1.50
    public double getRentalPrice(Rental r) {
        double total = BASE_RENTAL_AMOUNT;
        if (r._daysRented > MIN_DAYS_RENTED) {
            total += (r._daysRented - MIN_DAYS_RENTED) * RENTAL_MULTIPLIER;
        }
        return total;
    }
}
