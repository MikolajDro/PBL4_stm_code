# AT commands to communicate between stm32 and esp32

## Version v1.1

Commands from esp32 to stm32

- AT+DV?\n Device version, simple commend to check device version

  - Response: +Devise version v1.0

- AT\n Check if AT works

  - Response: +AT

- AT+ST=SSMMHHDMMYY\n Set time,

  - SS seconds 0-59

  - MM minutes 0-59

  - HH hours 0-23

  - D days 1-7 it’s doesn’t start with 0 because RTC by default works
    with 1 - 7

  - MM months 1-12, it’s doesn’t start with 0 because RTC by default
    works with 1 - 12

  - YY years 0-99

    - Response: +Set successfully

    - Response: +Set unsuccessfully

- AT+SA= SSMMHHDMMYY\n Set alarm

  - SS seconds 0-59

  - MM minutes 0-59

  - HH hours 0-23

  - D days 1-7 it’s doesn’t start with 0 because RTC by default works
    with 1 - 7

  - MM months 1-12, it’s doesn’t start with 0 because RTC by default
    works with 1 - 12

  - YY years 0-99

    - Response: +Set successfully

    - Response: +Set unsuccessfully

- AT+DA= SSMMHHDMMYY\n Delate alarm

  - SS seconds 0-59

  - MM minutes 0-59

  - HH hours 0-23

  - D days 1-7 it’s doesn’t start with 0 because RTC by default works
    with 1 - 7

  - MM months 1-12, it’s doesn’t start with 0 because RTC by default
    works with 1 - 12

  - YY years 0-99

    - Response: +Set successfully

    - Response: +Set unsuccessfully

- AT+PW\n Pour water

  - Response: +Successful

  - Response: +Unsuccessful

- AT+SW=XYZ\n Set weight XYZ is weight in BCD format

  - Response: +Successful

  - Response: +Unsuccessful

- AT+BL?\n Get battery level

  - Response: +XYZ xyz in BDC system

  - Response: +Unsuccessful

- AT+RT\n Restart µC

  - Response: +Successful

  - Response: +Unsuccessful
