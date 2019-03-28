# Multiplier With Optional Accumulation (MPY/MAC)
In this example we will use the Multiplier with optional accumulation to multiply two 32-bit operands into one 64-bit result. Since the PRU is a 32-bit processor doing a 32-bit multiplication can take a longer time while the MAC can do this in 1 clock pulse. This is important for our final product since we will be using fixed point number bigger than 32 bits.

Each core has its own unsigned multiplier with optional accumulation. These have 2 operation modes:

1. Multiply only (MPY)
2. Multiply and Accumulate (MAC)

The MAC is directly connected to the internal R25-R29 registers for each PRU. These are configured and controlled using the broadside load/store PRU interface and XFR instructions. All reads and writes using this interface/instructions are done in 1 clock pulse

- Configure in MPY/MAC mode using R25
- Load the 32-bit operands in R28/R29
- 64-bit result (with carry flag) in register R26/R27
- PRU broadside interface and XFR instructions for importing/exporting operands/results

We will only be using the multiply only mode since we will do not need the accumulation. The following steps are the steps needed performed by the PRU to perform one 32-bit multiplication.

1. Enable MAC mode:
  - Clear R25[0] for MAC mode
  - Store mode to MAC using XOUT with these parameters
    * Device ID = 0
    * Base reg = R25
    * Size = 1
2. Load the operands into R28/R29 using XIN
3. Delay for 1 clock cycle so the MAC can perform its multiplication
4. Load product into the PRU using XIN on R26/R27
5. Repeat steps 2-4 for each new multiplication
