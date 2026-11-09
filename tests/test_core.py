import unittest
from irrigation_scheduler import IrrigationController

class IrrigationTests(unittest.TestCase):
    def test_start_stop_cooldown(self):
        c = IrrigationController(max_run_seconds=10, cooldown_seconds=5)
        self.assertTrue(c.ingest(0, 20)["pump_on"])
        self.assertTrue(c.ingest(4, 40)["pump_on"])
        self.assertEqual(c.ingest(5, 60)["reason"], "target_reached")
        self.assertEqual(c.ingest(6, 10)["reason"], "cooldown")
        self.assertTrue(c.ingest(11, 10)["pump_on"])
        self.assertEqual(c.ingest(21, 10)["reason"], "max_runtime")
    def test_tank_interlock(self):
        c = IrrigationController()
        self.assertFalse(c.ingest(0, 0, False)["pump_on"])
        self.assertTrue(c.ingest(1, 0)["pump_on"])
        self.assertEqual(c.ingest(2, 0, False)["reason"], "tank_empty")
    def test_validation(self):
        c = IrrigationController()
        with self.assertRaises(ValueError):
            c.ingest(1, float("nan"))
        with self.assertRaises(ValueError):
            c.ingest(1, 30, "yes")
        with self.assertRaises(ValueError):
            IrrigationController(60, 40)
