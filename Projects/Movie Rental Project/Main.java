import java.io.*;
import java.util.Scanner;

public class Main {
    static String INPUT_FILE_NAME = "testdata.csv";
    public static void main(String[] args) throws Exception {
        /*
        Scanner consoleScanner = new Scanner(System.in);
        consoleScanner.close();

        Scanner fileScanner = null;
        FileInputStream inputFile = null;

        try {
            inputFile = new FileInputStream(INPUT_FILE_NAME);
            fileScanner = new Scanner(inputFile);
        } catch (IOException e) {
            System.out.println("Error: file could not be opened!");
            System.exit(1); 
        }
        */
        Scanner fileScanner = new Scanner(new File(INPUT_FILE_NAME));

        while (fileScanner.hasNextLine()) {
            String line = fileScanner.nextLine();
            
            String[] lineArray = line.split(",");
            String name = lineArray[0];
            if (!name.equals("Jake")) {
                continue;
            }
            Customer customer = new Customer(name);
            Movie movie = new Movie(lineArray[1]);
            String type = lineArray[2];
            int daysRented = Integer.parseInt(lineArray[3]);
            Rental rental;
            switch (type) {
                case "Regular" -> {
                    rental = new Rental(movie, daysRented, new FRPDefault(), new PriceRegular(), customer);
                }                    
                case "New Release" -> {
                    rental = new Rental(movie, daysRented, new FRPNewRelease(), new PriceNewRelease(), customer);
                }
                case "Childrens" -> {
                    rental = new Rental(movie, daysRented, new FRPDefault(), new PriceChildrens(), customer);
                }
                default -> {
                    throw new RuntimeException("Unrecognized movie type: " + type);
                }
            }
            customer.totalFrequentRenterPoints = 9;
            //DiscountedRental dr = new TenOffCoupon(new HalfOffCoupon(rental));
            DiscountedRental dr = new FRPCoupon(rental);
            customer.addRental(dr);
            Print print = new PrintString(customer);
            print.printStatement();
        }
        fileScanner.close();
    }
}
