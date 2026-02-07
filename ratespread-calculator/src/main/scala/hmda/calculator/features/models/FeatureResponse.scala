package hmda.calculator.features.models

case class FeatureResponse(
  refiIncentives: Seq[Double],
  penaltyPoints: Seq[Double],
  adjustedIncentives: Seq[Double]
)
