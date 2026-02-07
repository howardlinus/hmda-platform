package hmda.calculator.features.models

import java.time.LocalDate

case class FeatureRequest(
  loanTerm: Int,
  amortizationType: String,
  loanStartDate: LocalDate,
  penaltyPoints: Seq[Double] = Seq.empty
)
