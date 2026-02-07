package hmda.calculator.features.parser

import hmda.calculator.features.models.FeatureRequest
import org.scalatest.{ MustMatchers, WordSpec }
import java.time.LocalDate
import java.time.format.DateTimeFormatter

class FeatureConfigParserSpec extends WordSpec with MustMatchers {

  "FeatureConfigParser" must {

    "parse valid JSON string" in {
      val jsonString =
        """
          |{
          |  "loanTerm": 30,
          |  "amortizationType": "FixedRate",
          |  "loanStartDate": "2018-03-22",
          |  "numberOfPeriods": 5,
          |  "penaltyPoints": [0.1, 0.2, 0.15]
          |}
        """.stripMargin

      val result = FeatureConfigParser.parseConfigString(jsonString)

      result.isRight mustBe true
      val (request, numberOfPeriods) = result.right.get

      request.loanTerm mustBe 30
      request.amortizationType mustBe "FixedRate"
      request.loanStartDate mustBe LocalDate.parse("2018-03-22", DateTimeFormatter.ISO_LOCAL_DATE)
      request.penaltyPoints mustBe Seq(0.1, 0.2, 0.15)
      numberOfPeriods mustBe 5
    }

    "parse JSON with empty penalty points" in {
      val jsonString =
        """
          |{
          |  "loanTerm": 15,
          |  "amortizationType": "VariableRate",
          |  "loanStartDate": "2019-01-15",
          |  "numberOfPeriods": 3,
          |  "penaltyPoints": []
          |}
        """.stripMargin

      val result = FeatureConfigParser.parseConfigString(jsonString)

      result.isRight mustBe true
      val (request, numberOfPeriods) = result.right.get

      request.loanTerm mustBe 15
      request.amortizationType mustBe "VariableRate"
      request.penaltyPoints mustBe Seq.empty
      numberOfPeriods mustBe 3
    }

    "parse JSON with multiple penalty points" in {
      val jsonString =
        """
          |{
          |  "loanTerm": 20,
          |  "amortizationType": "FixedRate",
          |  "loanStartDate": "2020-06-10",
          |  "numberOfPeriods": 8,
          |  "penaltyPoints": [0.1, 0.2, 0.15, 0.1, 0.05, 0.02]
          |}
        """.stripMargin

      val result = FeatureConfigParser.parseConfigString(jsonString)

      result.isRight mustBe true
      val (request, numberOfPeriods) = result.right.get

      request.penaltyPoints mustBe Seq(0.1, 0.2, 0.15, 0.1, 0.05, 0.02)
      numberOfPeriods mustBe 8
    }

    "return error for invalid JSON" in {
      val jsonString = "{ invalid json }"

      val result = FeatureConfigParser.parseConfigString(jsonString)

      result.isLeft mustBe true
      result.left.get must include("Error decoding JSON")
    }

    "return error for missing field" in {
      val jsonString =
        """
          |{
          |  "loanTerm": 30,
          |  "amortizationType": "FixedRate",
          |  "loanStartDate": "2018-03-22"
          |}
        """.stripMargin

      val result = FeatureConfigParser.parseConfigString(jsonString)

      result.isLeft mustBe true
      result.left.get must include("Error decoding JSON")
    }

    "return error for invalid date format" in {
      val jsonString =
        """
          |{
          |  "loanTerm": 30,
          |  "amortizationType": "FixedRate",
          |  "loanStartDate": "22/03/2018",
          |  "numberOfPeriods": 5,
          |  "penaltyPoints": [0.1]
          |}
        """.stripMargin

      val result = FeatureConfigParser.parseConfigString(jsonString)

      result.isLeft mustBe true
      result.left.get must include("Error parsing date")
    }

    "parse JSON with different date" in {
      val jsonString =
        """
          |{
          |  "loanTerm": 25,
          |  "amortizationType": "FixedRate",
          |  "loanStartDate": "2021-12-31",
          |  "numberOfPeriods": 4,
          |  "penaltyPoints": [0.5]
          |}
        """.stripMargin

      val result = FeatureConfigParser.parseConfigString(jsonString)

      result.isRight mustBe true
      val (request, _) = result.right.get

      request.loanStartDate mustBe LocalDate.parse("2021-12-31", DateTimeFormatter.ISO_LOCAL_DATE)
    }

    "handle zero penalty points" in {
      val jsonString =
        """
          |{
          |  "loanTerm": 30,
          |  "amortizationType": "FixedRate",
          |  "loanStartDate": "2018-03-22",
          |  "numberOfPeriods": 3,
          |  "penaltyPoints": [0.0, 0.0, 0.0]
          |}
        """.stripMargin

      val result = FeatureConfigParser.parseConfigString(jsonString)

      result.isRight mustBe true
      val (request, _) = result.right.get

      request.penaltyPoints mustBe Seq(0.0, 0.0, 0.0)
    }

    "handle negative penalty points" in {
      val jsonString =
        """
          |{
          |  "loanTerm": 30,
          |  "amortizationType": "FixedRate",
          |  "loanStartDate": "2018-03-22",
          |  "numberOfPeriods": 2,
          |  "penaltyPoints": [-0.1, 0.2]
          |}
        """.stripMargin

      val result = FeatureConfigParser.parseConfigString(jsonString)

      result.isRight mustBe true
      val (request, _) = result.right.get

      request.penaltyPoints mustBe Seq(-0.1, 0.2)
    }

    "parse example config file format" in {
      val jsonString =
        """
          |{
          |  "loanTerm": 30,
          |  "amortizationType": "FixedRate",
          |  "loanStartDate": "2018-03-22",
          |  "numberOfPeriods": 8,
          |  "penaltyPoints": [0.1, 0.2, 0.15, 0.1, 0.05]
          |}
        """.stripMargin

      val result = FeatureConfigParser.parseConfigString(jsonString)

      result.isRight mustBe true
      val (request, numberOfPeriods) = result.right.get

      // This matches the example config file structure
      request.loanTerm mustBe 30
      request.amortizationType mustBe "FixedRate"
      request.loanStartDate mustBe LocalDate.parse("2018-03-22")
      numberOfPeriods mustBe 8
      request.penaltyPoints.length mustBe 5
    }
  }
}
