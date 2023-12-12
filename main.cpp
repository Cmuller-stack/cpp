/* -----------------------------------------------
Welcome to the Black-Scholes option price app
Application done by Cedric Muller. ®2023
--------------------------------------------------
*/

// Librairies
#include <iostream>
#include <cmath>
#include <fstream>
#include <sys/ioctl.h>  // For ioctl
#include <iomanip>
#include <unistd.h>


/* ANSI escape codes for text color you can go on this GitHub repository :
 https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a */

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define BGRN    "\e[1;32m"
#define BRED    "\e[1;31m"

// Function to get the terminal width
int getTerminalWidth() {
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return size.ws_col;
}

// Function to calculate the cumulative distribution function (CDF) of the standard normal distribution
double cumulativeDistributionFunction(double x) {
    return 0.5 * (1 + std::erf(x / std::sqrt(2.0)));
}

// Function to calculate the Black-Scholes option price
double calculateBlackScholesOption(double S, double K, double sigma, double T, double r, char optionType) {
    double d1 = (std::log(S / K) + (r + 0.5 * std::pow(sigma, 2)) * T) / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);

    if (optionType == 'c') {
        // Call option
        return S * cumulativeDistributionFunction(d1) - K * std::exp(-r * T) * cumulativeDistributionFunction(d2);
    } else if (optionType == 'p') {
        // Put option
        return K * std::exp(-r * T) * cumulativeDistributionFunction(-d2) - S * cumulativeDistributionFunction(-d1);
    } else {
        std::cerr << RED << "Invalid option type. Use 'c' for call option or 'p' for put option." << RESET << std::endl;
        return 0.0;
    }
}

// Function to calculate delta numerically
double calculateDeltaNumerically(double S, double K, double sigma, double T, double r, char optionType, double dS) {
    double optionPrice = calculateBlackScholesOption(S, K, sigma, T, r, optionType);
    double optionPriceUp = calculateBlackScholesOption(S + dS, K, sigma, T, r, optionType);
    return (optionPriceUp - optionPrice) / dS;
}

int main() {

    // Get the width of the terminal dynamically
    int terminalWidth = getTerminalWidth();

    // Text to center
    std::string TitleCenter = "Welcome to the Black-Scholes Option Pricing Model!";

    // Calculate padding on each side
    int padding = (terminalWidth - TitleCenter.length()) / 2;

    std::cout << GREEN << std::setw(padding) << "" << "------------------------------------------------"  << std::setw(padding) << "" << std::endl;
    std::cout << GREEN << std::setw(padding) << "" << "This is app is powered by Cedric Muller ® 2023"  << std::setw(padding) << "" << std::endl;
    std::cout << GREEN << std::setw(padding) << "" << "------------------------------------------------"  << std::setw(padding) << "" << std::endl;

    /*std::cout << GREEN << "------------------------------------------------" << std::endl;
    std::cout << GREEN << "This is app is powered by Cedric Muller ® 2023" << std::endl;
    std::cout << GREEN << "------------------------------------------------" << std::endl;*/
    
    // Output with padding
    std::cout << BRED << std::setw(padding) << "" << TitleCenter << std::setw(padding) << "" << std::endl;
    // std::cout << BRED << "Welcome to the Black-Scholes Option Pricing Model!" << std::endl;

    // Input parameters
    double stockPrice, strikePrice, volatility, timeToMaturity, interestRate;
    char optionType;

    // Get user input
    std::cout << BGRN << "Enter current stock price: " << RESET;
    std::cin >> stockPrice;

    std::cout << BGRN << "Enter option strike price: " << RESET;
    std::cin >> strikePrice;

    std::cout << BGRN << "Enter volatility (in decimal form, e.g., 0.2 for 20%): " << RESET;
    std::cin >> volatility;

    std::cout << BGRN << "Enter time to maturity (in years): " << RESET;
    std::cin >> timeToMaturity;

    std::cout << BGRN << "Enter risk-free interest rate (in decimal form, e.g., 0.05 for 5%): " << RESET;
    std::cin >> interestRate;

    std::cout << BGRN << "Enter option type ('c' for call, 'p' for put): " << RESET;
    std::cin >> optionType;

    // Simulation parameters
    const int numSteps = 100;
    const double deltaTime = timeToMaturity / numSteps;

    // Small change in stock price for numerical delta calculation
    double dS = stockPrice * 0.01;

    // Writing data to a file for Gnuplot
    std::ofstream dataFile("black_scholes_data.dat");

    // Simulate delta and option price over time
    for (int step = 0; step <= numSteps; ++step) {
        double currentTime = step * deltaTime;

        // Calculate Black-Scholes option price at each time step
        double optionPrice = calculateBlackScholesOption(stockPrice, strikePrice, volatility, currentTime, interestRate, optionType);

        // Calculate Delta (sensitivity to stock price changes) at each time step
        double delta = calculateDeltaNumerically(stockPrice, strikePrice, volatility, currentTime, interestRate, optionType, dS);

        // Write data to file
        dataFile << currentTime << " " << optionPrice << " " << delta << std::endl;

        // Display Greek values in the table
        std::cout << std::setfill(' ') << std::fixed << std::setprecision(4)
                  << std::setw(15) << currentTime
                  << std::setw(15) << optionPrice
                  << std::setw(15) << delta << "\n";
    }

    dataFile.close();

    // Plotting using Gnuplot
    std::ofstream gnuplotScript("plot_script.gnu");
    gnuplotScript << "set title 'Black-Scholes Option Pricing and Delta'\n";
    gnuplotScript << "set xlabel 'Time'\n";
    gnuplotScript << "set ylabel '" << YELLOW << "Option Price" << RESET << "'\n";
    gnuplotScript << "set y2label '" << YELLOW << "Delta" << RESET << "'\n";
    gnuplotScript << "set ytics nomirror\n";
    gnuplotScript << "set y2tics\n";
    gnuplotScript << "set grid\n";
    gnuplotScript << "plot 'black_scholes_data.dat' using 1:2 with lines title '" << YELLOW << "Option Price" << RESET << "' axes x1y1, "
                     "'black_scholes_data.dat' using 1:3 with lines title '" << YELLOW << "Delta" << RESET << "' axes x1y2" << std::endl;
    gnuplotScript.close();

    // Execute Gnuplot script
    system("gnuplot -persist plot_script.gnu");

    return 0;
}
