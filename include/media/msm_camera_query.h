#ifndef __LINUX_MSM_CAMERA_QUERY_H_
#define __LINUX_MSM_CAMERA_QUERY_H_

struct otp_info_t {
	uint8_t otp_info[256];
};

extern uint16_t af_type;
struct af_info_t {
	uint16_t af_liftoff_cal;
	uint16_t af_macro_cal;
	uint16_t af_inf_cal;
};
extern struct af_info_t af_info;

#endif /* __LINUX_MSM_CAMERA_QUERY_H_ */
