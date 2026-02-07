package hmda.calculator.features.parser

import hmda.calculator.features.models.FeatureRequest
import io.circe.{ Decoder, HCursor }
import io.circe.parser.decode
import java.time.LocalDate
import java.time.format.DateTimeFormatter
import scala.io.Source
import scala.util.{ Try, Success, Failure }

object FeatureConfigParser {

  case class FeatureConfig(
    loanTerm: Int,
    amortizationType: String,
    loanStartDate: String,
    numberOfPeriods: Int,
    penaltyPoints: Seq[Double]
  )

  implicit val featureConfigDecoder: Decoder[FeatureConfig] = new Decoder[FeatureConfig] {
    final def apply(c: HCursor): Decoder.Result[FeatureConfig] =
      for {
        loanTerm <- c.downField("loanTerm").as[Int]
        amortizationType <- c.downField("amortizationType").as[String]
        loanStartDate <- c.downField("loanStartDate").as[String]
        numberOfPeriods <- c.downField("numberOfPeriods").as[Int]
        penaltyPoints <- c.downField("penaltyPoints").as[Seq[Double]]
      } yield {
        FeatureConfig(loanTerm, amortizationType, loanStartDate, numberOfPeriods, penaltyPoints)
      }
  }

  /**
   * Parse JSON configuration file and return FeatureRequest and numberOfPeriods
   */
  def parseConfigFile(filePath: String): Either[String, (FeatureRequest, Int)] = {
    Try {
      val source = Source.fromFile(filePath)
      val jsonString = try source.mkString finally source.close()
      jsonString
    } match {
      case Success(jsonString) =>
        decode[FeatureConfig](jsonString) match {
          case Right(config) =>
            Try {
              val date = LocalDate.parse(config.loanStartDate, DateTimeFormatter.ISO_LOCAL_DATE)
              val request = FeatureRequest(
                loanTerm = config.loanTerm,
                amortizationType = config.amortizationType,
                loanStartDate = date,
                penaltyPoints = config.penaltyPoints
              )
              (request, config.numberOfPeriods)
            } match {
              case Success(result) => Right(result)
              case Failure(e) => Left(s"Error parsing date: ${e.getMessage}")
            }
          case Left(error) => Left(s"Error decoding JSON: ${error.getMessage}")
        }
      case Failure(e) => Left(s"Error reading file: ${e.getMessage}")
    }
  }

  /**
   * Parse JSON string and return FeatureRequest and numberOfPeriods
   */
  def parseConfigString(jsonString: String): Either[String, (FeatureRequest, Int)] = {
    decode[FeatureConfig](jsonString) match {
      case Right(config) =>
        Try {
          val date = LocalDate.parse(config.loanStartDate, DateTimeFormatter.ISO_LOCAL_DATE)
          val request = FeatureRequest(
            loanTerm = config.loanTerm,
            amortizationType = config.amortizationType,
            loanStartDate = date,
            penaltyPoints = config.penaltyPoints
          )
          (request, config.numberOfPeriods)
        } match {
          case Success(result) => Right(result)
          case Failure(e) => Left(s"Error parsing date: ${e.getMessage}")
        }
      case Left(error) => Left(s"Error decoding JSON: ${error.getMessage}")
    }
  }
}
