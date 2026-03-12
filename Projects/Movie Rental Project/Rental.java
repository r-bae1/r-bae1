
public class Rental implements DiscountedRental {
    Movie _movie;
    int _daysRented;
    FRPStrategy frpStrategy;
    PriceStrategy priceStrategy;
    Customer _customer;

    public Rental(Movie movie, int daysRented, FRPStrategy frpStrategy, PriceStrategy priceStrategy, Customer customer) {
        _movie = movie;
        _daysRented = daysRented;
        this.frpStrategy = frpStrategy;
        this.priceStrategy = priceStrategy;
        _customer = customer;
    }

    public int getDaysRented() {
        return _daysRented;
    }

    public Movie getMovie() {
        return _movie;
    }

    public int getFrequentRenterPoints() {
        return frpStrategy.frp(this);
    }

    public double getRentalPrice() {
        return priceStrategy.getRentalPrice(this);
    }

    @Override
    public String toString() {
        return rentalReceiptItem();
    }

    public Customer getCustomer() {
        return _customer;
    }

    // Returns a string with the details of a rental
    private String rentalReceiptItem() {
        // show figures for this rental
        String result = "\t" + this._movie.getTitle() +
                "\t" + String.valueOf(this.getRentalPrice()) + "\n";
        return result;
    }

    // Returns a string with the details of a rental in XML format
    public String rentalReceiptItemXMLFormat() {
        String xml = "<Rental>\n";
        xml += "<MovieTitle>" + this._movie.getTitle() + "</MovieTitle>\n";
        xml += "<RentalPrice>" + this.getRentalPrice() + "</RentalPrice>\n";
        //xml += "<RentalType>" + "</RentalType>\n";
        xml += "</Rental>\n";
        return xml;
    }
}

interface DiscountedRental {
    int getDaysRented();
    Movie getMovie();
    double getRentalPrice();
    int getFrequentRenterPoints();
    Customer getCustomer();
}

class Coupon implements DiscountedRental {
    DiscountedRental dr;
    public Coupon(DiscountedRental dr) {
        this.dr = dr;
    }

    @Override
    public int getDaysRented() {
        return dr.getDaysRented();
    }

    @Override
    public Movie getMovie() {
        return dr.getMovie();
    }

    @Override
    public double getRentalPrice() {
        return dr.getRentalPrice();
    }

    @Override
    public int getFrequentRenterPoints() {
        return dr.getFrequentRenterPoints();
    }

    @Override
    public String toString() {
        return dr.toString();
    }

    @Override
    public Customer getCustomer() {
        return dr.getCustomer();
    }
}

class HalfOffCoupon extends Coupon {
    public HalfOffCoupon(DiscountedRental dr) {
        super(dr);
    }

    @Override
    public double getRentalPrice() {
        return super.getRentalPrice() / 2;
    }
}

class TenOffCoupon extends Coupon {
    public TenOffCoupon(DiscountedRental dr) {
        super(dr);
    }

    @Override
    public double getRentalPrice() {
        double price = super.getRentalPrice();
        return price > 50 ? price - 10 : price;
    }
}

class FRPCoupon extends Coupon {
    public FRPCoupon(DiscountedRental dr) {
        super(dr);
    }

    @Override
    public double getRentalPrice() {
        if (dr.getCustomer().totalFrequentRenterPoints >= 10) {
            dr.getCustomer().totalFrequentRenterPoints -= 10;
            return 0;
        }
        return super.getRentalPrice();
    }
}
