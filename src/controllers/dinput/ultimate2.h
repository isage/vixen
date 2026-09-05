uint8_t ultimate2_processReport(Controller *c, size_t length)
{
  if (length < 11)
    return 0;

  process_dpad_angle(c, 1);

  // face buttons
  if (bit(c->buffer + 8, 0))
    c->controlData.buttons |= SCE_CTRL_CROSS;
  if (bit(c->buffer + 8, 1))
    c->controlData.buttons |= SCE_CTRL_CIRCLE;
  if (bit(c->buffer + 8, 3))
    c->controlData.buttons |= SCE_CTRL_SQUARE;
  if (bit(c->buffer + 8, 4))
    c->controlData.buttons |= SCE_CTRL_TRIANGLE;

  // bumpers
  if (bit(c->buffer + 8, 6))
    c->controlData.buttons |= SCE_CTRL_L1;
  if (bit(c->buffer + 8, 7))
    c->controlData.buttons |= SCE_CTRL_R1;

  // triggers
  c->controlData.lt = c->buffer[7];
  c->controlData.rt = c->buffer[6];

  if (bit(c->buffer + 9, 0))
    c->controlData.buttons |= SCE_CTRL_LTRIGGER;
  if (bit(c->buffer + 9, 1))
    c->controlData.buttons |= SCE_CTRL_RTRIGGER;

  // select, start and home
  if (bit(c->buffer + 9, 2))
    c->controlData.buttons |= SCE_CTRL_SELECT;
  if (bit(c->buffer + 9, 3))
    c->controlData.buttons |= SCE_CTRL_START;
  if (bit(c->buffer + 9, 4))
    c->controlData.buttons |= SCE_CTRL_PSBUTTON;

  // stick clicks
  if (bit(c->buffer + 9, 5))
    c->controlData.buttons |= SCE_CTRL_L3;
  if (bit(c->buffer + 9, 6))
    c->controlData.buttons |= SCE_CTRL_R3;

  // sticks
  c->controlData.leftX  = c->buffer[2];
  c->controlData.leftY  = c->buffer[3];
  c->controlData.rightX = c->buffer[4];
  c->controlData.rightY = c->buffer[5];

  return 1;
}
