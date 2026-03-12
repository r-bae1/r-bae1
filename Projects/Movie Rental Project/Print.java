public abstract class Print {
    protected Customer _customer;
    protected String   _statement = "";
    
    public Print(Customer customer) {
        _customer = customer;
    }

    public String getStatement() {
        return _statement;
    }

    protected abstract void setStatement();
    protected abstract void getHeader();
    protected abstract void getFooter();

    public void printStatement() {
        setStatement();
        System.out.println(_statement);
    }
}

class PrintString extends Print {
    public PrintString(Customer customer) {
        super(customer);
    }

    @Override
    protected void setStatement() {
        getHeader();
        // Iterates through the list of rentals and appends each rental's details to the receipt
        for (DiscountedRental dr : _customer.getRentals()) {
            _statement += dr.toString();
        }
        getFooter();
    }

    @Override
    protected void getHeader() {
        // String to be returned starts with a header for the customer's name
        _statement = "Rental Record for " + _customer.getName() + "\n";
    }

    @Override
    protected void getFooter() {
        // Appends the footer of the receipt with the total amount owed and the total frequent renter points
        _statement += "Amount owed is " + _customer.getTotalAmountOwed() + "\n";
        _statement += "You earned " + _customer.getTotalFrequentRenterPoints() + " frequent renter points";
    }
}

/*
class PrintXML extends Print {
    public PrintXML(Customer customer) {
        super(customer);
    }

    @Override
    protected void setStatement() {
        getHeader();
        // Iterates through the list of rentals and appends each rental's details to the receipt
        for (Rental dr : _customer.getRentals()) {
            _statement += dr.rental.rentalReceiptItemXMLFormat();
        }
        getFooter();
    }

    @Override
    protected void getHeader() {
        // String to be returned starts with a header for the customer's name
        _statement = "<Customer>\n";
        _statement += "<Name>" + _customer.getName() + "</Name>\n";
        _statement += "<Rentals>\n";
    }

    @Override
    protected void getFooter() {
        // Appends the footer of the receipt with the total amount owed and the total frequent renter points
        _statement += "</Rentals>\n";
        
        _statement += "<TotalAmountOwed>" + _customer.getTotalAmountOwed() + "</TotalAmountOwed>\n";
        _statement += "<TotalFrequentRenterPoints>" + _customer.getTotalFrequentRenterPoints() + "</TotalFrequentRenterPoints>\n";
        _statement += "</Customer>\n";
    }
}
*/