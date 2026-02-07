package hmda.calculator.features

import hmda.calculator.apor.{ APOR, AporListEntity, AporOperation, FixedRate, VariableRate }
import hmda.calculator.features.models.{ FeatureRequest, FeatureResponse }
import org.scalatest.{ MustMatchers, WordSpec }
import java.time.LocalDate
import java.time.format.DateTimeFormatter

class FeatureBuilderSpec extends WordSpec with MustMatchers {

  val lockInDate =
    LocalDate.parse("2018-03-22", DateTimeFormatter.ISO_LOCAL_DATE)

  // Setup test APOR data
  val exampleAPOR: APOR = APOR(
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

  "FeatureBuilder" must {
    
    "pad penalty points with zeros when fewer than periods" in {
      val penaltyPoints = Seq(0.1, 0.2, 0.15)
      val numberOfPeriods = 5
      
      val padded = FeatureBuilder.padPenaltyPoints(penaltyPoints, numberOfPeriods)
      
      padded mustBe Seq(0.1, 0.2, 0.15, 0.0, 0.0)
      padded.length mustBe numberOfPeriods
    }

    "not pad penalty points when equal to periods" in {
      val penaltyPoints = Seq(0.1, 0.2, 0.15, 0.05, 0.08)
      val numberOfPeriods = 5
      
      val padded = FeatureBuilder.padPenaltyPoints(penaltyPoints, numberOfPeriods)
      
      padded mustBe penaltyPoints
      padded.length mustBe numberOfPeriods
    }

    "truncate penalty points when more than periods" in {
      val penaltyPoints = Seq(0.1, 0.2, 0.15, 0.05, 0.08, 0.03, 0.02)
      val numberOfPeriods = 5
      
      val padded = FeatureBuilder.padPenaltyPoints(penaltyPoints, numberOfPeriods)
      
      padded mustBe Seq(0.1, 0.2, 0.15, 0.05, 0.08)
      padded.length mustBe numberOfPeriods
    }

    "pad penalty points with all zeros when empty list" in {
      val penaltyPoints = Seq.empty[Double]
      val numberOfPeriods = 3
      
      val padded = FeatureBuilder.padPenaltyPoints(penaltyPoints, numberOfPeriods)
      
      padded mustBe Seq(0.0, 0.0, 0.0)
      padded.length mustBe numberOfPeriods
    }

    "calculate refi_incentive correctly for single period" in {
      val request = FeatureRequest(
        loanTerm = 30,
        amortizationType = "FixedRate",
        loanStartDate = lockInDate,
        penaltyPoints = Seq.empty
      )

      val response = FeatureBuilder.buildFeatures(request, 1)
      
      // For the same date (period 0), refi_incentive should be 0 (same benchmark)
      response.refiIncentives.length mustBe 1
      response.refiIncentives.head mustBe 0.0
    }

    "build features with penalty points and refi_incentive" in {
      val request = FeatureRequest(
        loanTerm = 30,
        amortizationType = "FixedRate",
        loanStartDate = lockInDate,
        penaltyPoints = Seq(0.1, 0.2, 0.15)
      )

      val response = FeatureBuilder.buildFeatures(request, 5)
      
      // Check that penalty points are padded correctly
      response.penaltyPoints mustBe Seq(0.1, 0.2, 0.15, 0.0, 0.0)
      
      // Check that we have correct number of refi_incentives
      response.refiIncentives.length mustBe 5
      
      // Check that adjusted incentives are calculated
      response.adjustedIncentives.length mustBe 5
      
      // Adjusted incentive = refi_incentive - penalty_point
      response.adjustedIncentives.zip(response.refiIncentives.zip(response.penaltyPoints)).foreach {
        case (adjusted, (refi, penalty)) =>
          adjusted mustBe (refi - penalty)
      }
    }

    "handle empty penalty points list" in {
      val request = FeatureRequest(
        loanTerm = 30,
        amortizationType = "FixedRate",
        loanStartDate = lockInDate,
        penaltyPoints = Seq.empty
      )

      val response = FeatureBuilder.buildFeatures(request, 3)
      
      // Penalty points should be all zeros
      response.penaltyPoints mustBe Seq(0.0, 0.0, 0.0)
      
      // Adjusted incentives should equal refi_incentives when penalty is 0
      response.adjustedIncentives mustBe response.refiIncentives
    }

    "handle variable rate amortization type" in {
      val request = FeatureRequest(
        loanTerm = 30,
        amortizationType = "VariableRate",
        loanStartDate = lockInDate,
        penaltyPoints = Seq(0.1)
      )

      val response = FeatureBuilder.buildFeatures(request, 3)
      
      // Should work with VariableRate too
      response.penaltyPoints mustBe Seq(0.1, 0.0, 0.0)
      response.refiIncentives.length mustBe 3
      response.adjustedIncentives.length mustBe 3
    }

    "calculate correct adjusted incentives" in {
      val request = FeatureRequest(
        loanTerm = 30,
        amortizationType = "FixedRate",
        loanStartDate = lockInDate,
        penaltyPoints = Seq(0.1, 0.2)
      )

      val response = FeatureBuilder.buildFeatures(request, 3)
      
      // Verify the relationship: adjusted = refi - penalty
      response.adjustedIncentives(0) mustBe (response.refiIncentives(0) - 0.1)
      response.adjustedIncentives(1) mustBe (response.refiIncentives(1) - 0.2)
      response.adjustedIncentives(2) mustBe (response.refiIncentives(2) - 0.0)
    }

    "handle multiple periods correctly" in {
      val request = FeatureRequest(
        loanTerm = 30,
        amortizationType = "FixedRate",
        loanStartDate = lockInDate,
        penaltyPoints = Seq(0.05, 0.10, 0.15, 0.20, 0.25, 0.30)
      )

      val response = FeatureBuilder.buildFeatures(request, 8)
      
      // Penalty points should be padded to 8 periods
      response.penaltyPoints.length mustBe 8
      response.penaltyPoints.take(6) mustBe Seq(0.05, 0.10, 0.15, 0.20, 0.25, 0.30)
      response.penaltyPoints.drop(6) mustBe Seq(0.0, 0.0)
      
      // All outputs should have 8 values
      response.refiIncentives.length mustBe 8
      response.adjustedIncentives.length mustBe 8
    }
  }
}
