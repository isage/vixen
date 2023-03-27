#ifndef __DPAD_ANGLE_H__
#define __DPAD_ANGLE_H__

inline void process_dpad_angle(Controller *c, int idx)
{
    uint8_t dpad = c->buffer[idx] & 0x0F;
    switch (dpad)
    {
      case 0x0:
        c->controlData.buttons |= SCE_CTRL_UP;
        break;
      case 0x1:
        c->controlData.buttons |= SCE_CTRL_UP;
        c->controlData.buttons |= SCE_CTRL_RIGHT;
        break;
      case 0x2:
        c->controlData.buttons |= SCE_CTRL_RIGHT;
        break;
      case 0x3:
        c->controlData.buttons |= SCE_CTRL_RIGHT;
        c->controlData.buttons |= SCE_CTRL_DOWN;
        break;
      case 0x4:
        c->controlData.buttons |= SCE_CTRL_DOWN;
        break;
      case 0x5:
        c->controlData.buttons |= SCE_CTRL_DOWN;
        c->controlData.buttons |= SCE_CTRL_LEFT;
        break;
      case 0x6:
        c->controlData.buttons |= SCE_CTRL_LEFT;
        break;
      case 0x7:
        c->controlData.buttons |= SCE_CTRL_LEFT;
        c->controlData.buttons |= SCE_CTRL_UP;
        break;
      case 0x8:
      default:
        break;
    }
}

#endif