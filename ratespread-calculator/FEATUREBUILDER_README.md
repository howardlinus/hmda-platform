# FeatureBuilder - Penalty Point and Refi Incentive Calculator

## Overview

The FeatureBuilder is a component for calculating refinancing incentives with penalty point adjustments. It accepts a list of penalty point values (one for each period) and calculates the `refi_incentive` (the difference between benchmark at running quarter and benchmark at loan rate start) for each period. The penalty points are then subtracted from the refi_incentive to produce adjusted incentives.

## Key Features

- **Penalty Point Padding**: Automatically pads the penalty point list with zeros if fewer values are provided than the number of periods
- **Refi Incentive Calculation**: Calculates the difference between the APOR benchmark at each running quarter and the benchmark at loan start
- **JSON Configuration**: Supports loading configuration from JSON files or strings
- **Type Safety**: Uses Scala case classes for type-safe data modeling

## Components

### Model Classes

- **FeatureRequest**: Input parameters including loan term, amortization type, loan start date, and penalty points
- **FeatureResponse**: Output containing refi_incentives, penalty points (padded), and adjusted incentives

### Core Logic

- **FeatureBuilder**: Main object that orchestrates feature calculations
- **FeatureConfigParser**: JSON parser for loading configuration from files or strings

## Usage

### Example JSON Configuration

```json
{
  "loanTerm": 30,
  "amortizationType": "FixedRate",
  "loanStartDate": "2018-03-22",
  "numberOfPeriods": 8,
  "penaltyPoints": [0.1, 0.2, 0.15, 0.1, 0.05]
}
```

### Scala Code Example

```scala
import hmda.calculator.features.FeatureBuilder
import hmda.calculator.features.parser.FeatureConfigParser

// Parse JSON configuration
val jsonConfig = """{ ... }"""
FeatureConfigParser.parseConfigString(jsonConfig) match {
  case Right((request, numberOfPeriods)) =>
    // Build features
    val response = FeatureBuilder.buildFeatures(request, numberOfPeriods)
    
    // Access results
    println(s"Penalty Points (padded): ${response.penaltyPoints}")
    println(s"Refi Incentives: ${response.refiIncentives}")
    println(s"Adjusted Incentives: ${response.adjustedIncentives}")
    
  case Left(error) =>
    println(s"Error: $error")
}
```

### Using a Configuration File

```scala
// Parse from file
FeatureConfigParser.parseConfigFile("path/to/feature_config.json") match {
  case Right((request, numberOfPeriods)) =>
    val response = FeatureBuilder.buildFeatures(request, numberOfPeriods)
    // Process response...
    
  case Left(error) =>
    println(s"Error: $error")
}
```

## Calculation Logic

### 1. Penalty Point Padding

If the user provides fewer penalty points than the number of periods, the list is padded with zeros:

```
Input:  [0.1, 0.2, 0.15]
Periods: 5
Output: [0.1, 0.2, 0.15, 0.0, 0.0]
```

If more penalty points are provided than periods, the list is truncated.

### 2. Refi Incentive Calculation

For each period, the refi_incentive is calculated as:

```
refi_incentive[period] = benchmark[running_quarter] - benchmark[loan_start]
```

Where:
- `benchmark[running_quarter]` is the APOR (Average Prime Offer Rate) for the date at the current period (quarterly intervals from loan start)
- `benchmark[loan_start]` is the APOR at the loan origination date

### 3. Adjusted Incentive Calculation

The adjusted incentive subtracts the penalty point from the refi_incentive:

```
adjusted_incentive[period] = refi_incentive[period] - penalty_point[period]
```

## Running the Example

An example application is provided to demonstrate the usage:

```bash
sbt "project ratespread-calculator" "runMain hmda.calculator.features.examples.FeatureBuilderExample"
```

## Testing

Comprehensive unit tests are provided:

```bash
sbt "project ratespread-calculator" test
```

Test files:
- `FeatureBuilderSpec.scala` - Tests for the main FeatureBuilder logic
- `FeatureConfigParserSpec.scala` - Tests for JSON parsing

## Implementation Details

### Design Decisions

1. **JSON at Application Level**: The penalty_point values are provided through JSON configuration at the application level, not parsed from JSON in library-level code. This keeps the library code clean and focused on calculations.

2. **Immutable Data Structures**: All data models use immutable Scala case classes for thread safety and functional programming best practices.

3. **Error Handling**: The JSON parser returns `Either[String, (FeatureRequest, Int)]` to handle errors gracefully without exceptions.

4. **Quarterly Periods**: The calculation assumes quarterly periods (3-month intervals) for running quarter calculations.

### Dependencies

The FeatureBuilder relies on:
- Circe for JSON parsing
- Existing APOR (Average Prime Offer Rate) infrastructure in `hmda.calculator.apor`
- Java 8 Time API for date handling

## File Structure

```
ratespread-calculator/
├── examples/
│   └── feature_config.json                      # Example configuration file
└── src/
    ├── main/scala/hmda/calculator/features/
    │   ├── FeatureBuilder.scala                 # Main feature builder logic
    │   ├── models/
    │   │   ├── FeatureRequest.scala             # Input model
    │   │   └── FeatureResponse.scala            # Output model
    │   ├── parser/
    │   │   └── FeatureConfigParser.scala        # JSON parser
    │   └── examples/
    │       └── FeatureBuilderExample.scala      # Example application
    └── test/scala/hmda/calculator/features/
        ├── FeatureBuilderSpec.scala             # Feature builder tests
        └── FeatureConfigParserSpec.scala        # Parser tests
```

## API Reference

### FeatureBuilder.buildFeatures

```scala
def buildFeatures(request: FeatureRequest, numberOfPeriods: Int): FeatureResponse
```

Builds features including refi_incentive and penalty_point adjustments.

**Parameters:**
- `request`: FeatureRequest with loan parameters and penalty points
- `numberOfPeriods`: Number of periods to calculate features for

**Returns:** FeatureResponse with calculated features

### FeatureConfigParser.parseConfigFile

```scala
def parseConfigFile(filePath: String): Either[String, (FeatureRequest, Int)]
```

Parses a JSON configuration file.

**Parameters:**
- `filePath`: Path to the JSON configuration file

**Returns:** Either an error message (Left) or a tuple of (FeatureRequest, numberOfPeriods) (Right)

### FeatureConfigParser.parseConfigString

```scala
def parseConfigString(jsonString: String): Either[String, (FeatureRequest, Int)]
```

Parses a JSON configuration string.

**Parameters:**
- `jsonString`: JSON configuration as a string

**Returns:** Either an error message (Left) or a tuple of (FeatureRequest, numberOfPeriods) (Right)
