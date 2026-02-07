package hmda.calculator.features

import hmda.calculator.apor.{ APOR, APORCommands, RateType, FixedRate, VariableRate }
import hmda.calculator.features.models.{ FeatureRequest, FeatureResponse }
import java.time.LocalDate
import java.time.temporal.ChronoUnit

object FeatureBuilder {

  /**
   * Build features including refi_incentive and penalty_point adjustments
   * @param request Feature request with loan parameters and penalty points
   * @param numberOfPeriods Number of periods to calculate features for
   * @return FeatureResponse with calculated features
   */
  def buildFeatures(request: FeatureRequest, numberOfPeriods: Int): FeatureResponse = {
    // 1. Pad penalty points with zeros if needed
    val paddedPenaltyPoints = padPenaltyPoints(request.penaltyPoints, numberOfPeriods)
    
    // 2. Get the rate type
    val rateType = APORCommands.findRateType(request.amortizationType)
    
    // 3. Calculate benchmark at loan start
    val startBenchmark = getBenchmarkForDate(request.loanStartDate, request.loanTerm, rateType)
    
    // 4. Calculate refi_incentive for each period (benchmark difference)
    val refiIncentives = calculateRefiIncentives(
      request.loanStartDate,
      request.loanTerm,
      rateType,
      startBenchmark,
      numberOfPeriods
    )
    
    // 5. Subtract penalty points from refi_incentives
    val adjustedIncentives = refiIncentives.zip(paddedPenaltyPoints).map {
      case (incentive, penalty) => incentive - penalty
    }
    
    FeatureResponse(
      refiIncentives = refiIncentives,
      penaltyPoints = paddedPenaltyPoints,
      adjustedIncentives = adjustedIncentives
    )
  }

  /**
   * Pad penalty points list with zeros if fewer values than periods
   */
  private[features] def padPenaltyPoints(penaltyPoints: Seq[Double], numberOfPeriods: Int): Seq[Double] = {
    if (penaltyPoints.length >= numberOfPeriods) {
      penaltyPoints.take(numberOfPeriods)
    } else {
      penaltyPoints ++ Seq.fill(numberOfPeriods - penaltyPoints.length)(0.0)
    }
  }

  /**
   * Get benchmark (APOR) value for a specific date and loan term
   */
  private[features] def getBenchmarkForDate(
    date: LocalDate,
    loanTerm: Int,
    rateType: RateType
  ): Option[Double] = {
    APORCommands.aporForDateAndLoanTerm(loanTerm, rateType, date)
  }

  /**
   * Calculate refi_incentive for each period
   * refi_incentive = benchmark at running quarter - benchmark at loan start
   */
  private[features] def calculateRefiIncentives(
    startDate: LocalDate,
    loanTerm: Int,
    rateType: RateType,
    startBenchmark: Option[Double],
    numberOfPeriods: Int
  ): Seq[Double] = {
    startBenchmark match {
      case Some(startRate) =>
        (0 until numberOfPeriods).map { period =>
          // Calculate date for this period (quarterly)
          val periodDate = startDate.plus(period * 3, ChronoUnit.MONTHS)
          val periodBenchmark = getBenchmarkForDate(periodDate, loanTerm, rateType)
          
          periodBenchmark match {
            case Some(periodRate) =>
              // refi_incentive = benchmark at running quarter - benchmark at loan start
              BigDecimal(periodRate - startRate).setScale(3, BigDecimal.RoundingMode.HALF_UP).toDouble
            case None => 0.0 // If no benchmark available, use 0
          }
        }
      case None =>
        // If no start benchmark, all incentives are 0
        Seq.fill(numberOfPeriods)(0.0)
    }
  }
}
