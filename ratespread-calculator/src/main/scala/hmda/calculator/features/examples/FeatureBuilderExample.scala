package hmda.calculator.features.examples

import hmda.calculator.features.FeatureBuilder
import hmda.calculator.features.parser.FeatureConfigParser
import hmda.calculator.apor.{ APOR, AporListEntity, AporOperation, FixedRate, VariableRate }
import java.time.LocalDate
import java.time.format.DateTimeFormatter

/**
 * Example application demonstrating how to use FeatureBuilder with JSON configuration
 * 
 * Usage:
 * 1. Create a JSON configuration file with loan parameters and penalty points
 * 2. Parse the JSON file using FeatureConfigParser
 * 3. Pass the parsed configuration to FeatureBuilder.buildFeatures
 * 4. Get back calculated refi_incentives, penalty points, and adjusted incentives
 */
object FeatureBuilderExample {

  def main(args: Array[String]): Unit = {
    println("=== FeatureBuilder Example ===\n")

    // Setup example APOR data for demonstration
    setupExampleAporData()

    // Example 1: Parse JSON configuration from string
    val jsonConfig =
      """
        |{
        |  "loanTerm": 30,
        |  "amortizationType": "FixedRate",
        |  "loanStartDate": "2018-03-22",
        |  "numberOfPeriods": 8,
        |  "penaltyPoints": [0.1, 0.2, 0.15, 0.1, 0.05]
        |}
      """.stripMargin

    println("Example 1: Using JSON configuration string")
    println("JSON configuration:")
    println(jsonConfig)
    println()

    FeatureConfigParser.parseConfigString(jsonConfig) match {
      case Right((request, numberOfPeriods)) =>
        println(s"Parsed configuration:")
        println(s"  Loan Term: ${request.loanTerm}")
        println(s"  Amortization Type: ${request.amortizationType}")
        println(s"  Loan Start Date: ${request.loanStartDate}")
        println(s"  Number of Periods: $numberOfPeriods")
        println(s"  Penalty Points (input): ${request.penaltyPoints.mkString(", ")}")
        println()

        // Build features
        val response = FeatureBuilder.buildFeatures(request, numberOfPeriods)

        println("Feature Builder Results:")
        println(s"  Penalty Points (padded): ${response.penaltyPoints.mkString(", ")}")
        println(s"  Refi Incentives: ${response.refiIncentives.map(v => f"$v%.3f").mkString(", ")}")
        println(s"  Adjusted Incentives: ${response.adjustedIncentives.map(v => f"$v%.3f").mkString(", ")}")
        println()

        // Show period-by-period breakdown
        println("Period-by-Period Breakdown:")
        response.refiIncentives.indices.foreach { period =>
          val refi = response.refiIncentives(period)
          val penalty = response.penaltyPoints(period)
          val adjusted = response.adjustedIncentives(period)
          println(f"  Period $period: refi=$refi%.3f, penalty=$penalty%.3f, adjusted=$adjusted%.3f")
        }

      case Left(error) =>
        println(s"Error parsing configuration: $error")
    }

    println("\n=== Example 2: Using a file path ===")
    println("To use a JSON file, call:")
    println("  FeatureConfigParser.parseConfigFile(\"path/to/feature_config.json\")")
    println()
    println("The example configuration file is at:")
    println("  ratespread-calculator/examples/feature_config.json")
  }

  /**
   * Setup example APOR data for demonstration purposes
   */
  private def setupExampleAporData(): Unit = {
    val lockInDate = LocalDate.parse("2018-03-22", DateTimeFormatter.ISO_LOCAL_DATE)

    val exampleAPOR = APOR(
      lockInDate,
      Seq(1.01, 1.02, 1.03, 1.04, 1.05, 1.06, 1.07, 1.08, 1.09, 1.1, 1.11, 1.12, 1.13, 1.14, 1.15, 1.16, 1.17, 1.18, 1.19, 1.2, 1.21, 1.22,
        1.23, 1.24, 1.25, 1.26, 1.27, 1.28, 1.29, 1.3, 1.31, 1.32, 1.33, 1.34, 1.35, 1.36, 1.37, 1.38, 1.39, 1.40, 1.41, 1.42, 1.43, 1.44,
        1.45, 1.46, 1.47, 1.48, 1.49, 1.5)
    )

    AporOperation(exampleAPOR, FixedRate)
    AporOperation(
      APOR(
        lockInDate,
        Seq(2.01, 2.02, 2.03, 2.04, 2.05, 2.06, 2.07, 2.08, 2.09, 2.1, 2.11, 2.12, 2.13, 2.14, 2.15, 2.16, 2.17, 2.18, 2.19, 2.2, 2.21, 2.22,
          2.23, 2.24, 2.25, 2.26, 2.27, 2.28, 2.29, 2.3, 2.31, 2.32, 2.33, 2.34, 2.35, 2.36, 2.37, 2.38, 2.39, 2.40, 2.41, 2.42, 2.43, 2.44,
          2.45, 2.46, 2.47, 2.48, 2.49, 2.5)
      ),
      VariableRate
    )
  }
}
