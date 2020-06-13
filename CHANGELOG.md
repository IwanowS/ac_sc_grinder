1.1.1 / 2020-06-13
------------------

- Fix build error, caused by bug in PlaformIO registry.
- Fix PID_I component disable (regression in 1.1.0).
- Tune calibrator options to provide more stable result.


1.1.0 / 2020-04-29
------------------

- Full refactoring, prior to start more deep changes. Nothing "visible for
  users", but internals improved significantly.
- Disabled regulator's integral component by default.
- Moved triac's logic to ISR & use message queue to emit ADC data. Now logic
  is robust to theoretic computation bursts in event loop.
- Use hand-made "YIELD" macro to improve calibrator's logic readability.
- Renamed classes and reorganized helpers.
- Reduced number of divisions in single event loop pass (use table lookups +
  multiplication for small value ranges).
- Reworked stm32cubemx integration. Keep full sources to avoid conflicts with
  outdated PlatformIO's version. Removed all places with tight-coupled code.
- Added alternate HAL entry for new upcoming PCB.
- New EEPROM emulator.
- Added build tests on Travis-CI for MacOS & Win.


1.0.0 / 2019-08-01
------------------

- First Release
