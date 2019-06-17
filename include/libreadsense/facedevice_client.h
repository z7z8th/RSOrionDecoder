#ifndef FACE_DEVICE_CLIENT_H_
#define FACE_DEVICE_CLIENT_H_

/**
 * @file    face_device_client.h
 * @brief   face device client C API
 * @author  wei.he@readsense.cn
 * @version v1.0.0.0
 * @date    2018/12/14
 */

#include "rs_def.h"

BEGIN_EXTERN_C

#include "rs_device.h"
#include "rs_person.h"
#include "rs_recog.h"
#include "rs_gate.h"
#include "rs_misc.h"

/*
 * @callback connect_state_cb
 * @param[in] state 0: disconnected 1: connected
 * @param[in] userdata
 */
typedef int (CALLSPEC *connect_state_cb)(int state, void* userdata);

/*
 * @callback get_person_list_cb
 * @param[out] plist
 * @param[in] userdata
 */
typedef int (CALLSPEC *get_person_list_cb)(rs_person_list* plist, void* userdata);

/*
 * @callback person_cb
 * @param[in|out] dev
 * @param[in] userdata
 */
typedef int (CALLSPEC *person_cb)(rs_person* person, void* userdata);

/*
 * @callback clear_facedb_cb
 * @param[in] userdata
 */
typedef int (CALLSPEC *clear_facedb_cb)(void* userdata);

/*
 * @callback set_recog_mode_cb
 * @param[in] mode
 * @param[in] userdata
 */
typedef int (CALLSPEC *set_recog_mode_cb)(rs_recog_mode mode, void* userdata);

/*
 * @callback gate_control_cb
 * @param[in] gate
 * @param[in] userdata
 */
typedef int (CALLSPEC *gate_control_cb)(rs_gate_instruction* gate, void* userdata);

/*
 * @callback custom_request_cb
 * @param[in] request
 * @param[out] response
 * @param[in] userdata
 */
typedef int (CALLSPEC *custom_request_cb)(rs_custom_request* req, rs_custom_response* res, void* userdata);

STRUCT(person_cbs) {
    person_cb   add_person;
    person_cb   get_person;
    person_cb   update_person;
    person_cb   del_person;
    get_person_list_cb  get_person_list;
    clear_facedb_cb     clear_facedb;
};

/**
 * @fn      fdev_version
 * @brief   get API version
 * @param   null
 * @return  version string
 */
RS_API const char* CALLSPEC fdev_version();

/**
 * @fn      fdev_strerr
 * @brief   get error string
 * @param[in]   errcode
 * @return  error string
 */
RS_API const char* CALLSPEC fdev_strerr(int errcode);

/**
 * @fn      fdev_init
 * @brief   init fdev library
 * @param[out] ph: handle pointer
 * @return  0 OK, other error
 */
RS_API int CALLSPEC fdev_init(RShandle* ph);

/**
 * @fn      fdev_cleanup
 * @brief   cleanup fdev library
 * @param[in] ph: fdev_init handle
 * @return  null
 */
RS_API void CALLSPEC fdev_cleanup(RShandle* ph);

/**
 * @fn      fdev_set_srvaddr
 * @brief   set server address
 * @param[in] h: fdev_init handle
 * @param[in] ip
 * @param[in] port
 * @return  0 OK, other error
 */
RS_API int CALLSPEC fdev_set_srvaddr(RShandle h, const char* ip, int port);

/**
 * @fn      fdev_set_device_info
 * @param[in] h: fdev_init handle
 * @param[in] dev: device info struct
 * @return  0 OK, other error
 */
RS_API int CALLSPEC fdev_set_device_info(RShandle h, rs_device* dev);

/**
 * @fn      fdev_connect
 * @brief   connect server use tcp keep-alive
 * @param[in] h: fdev_init handle
 * @param[in] auto_reconnect: if true, auto reconnect when unexpected disconnect.
 * @return  0 OK, other error
 */
RS_API int CALLSPEC fdev_connect(RShandle h, int auto_reconnect DEFAULT(1));

/**
 * @fn      fdev_disconnect
 * @brief   disconnect with server
 * @param[in] h: fdev_init handle
 * @return  0 OK, other error
 */
RS_API int CALLSPEC fdev_disconnect(RShandle h);

/**
 * @fn      fdev_send_recog_record
 * @brief   send recognition record to server
 * @param[in] h: fdev_init handle
 * @param[in] record: recognition record
 * @return  0 OK, other error
 */
RS_API int CALLSPEC fdev_send_recog_record(RShandle h, rs_recog_record* record);

/**
 * @fn      fdev_send_gate_state
 * @brief   send gate state to server
 * @param[in] h: fdev_init handle
 * @param[in] state: gate state
 * @return  0 OK, other error
 */
RS_API int CALLSPEC fdev_send_gate_state(RShandle h, rs_gate_state* state);

/*
 * @fn      fdev_send_custom_event
 * @brief   send custom event
 * @param[in] h: fdev_init handle
 * @param[in] event
 * @return  0 OK, other error
 */
RS_API int CALLSPEC fdev_send_custom_event(RShandle h, rs_custom_event* event);

/*
 * @fn      fdev_setcb_connect_state
 * @brief   set connect state callback
 * @param[in] h: fdev_init handle
 * @param[in] cb: callback function
 * @param[in] userdata: callback function userdata
 */
RS_API int CALLSPEC fdev_setcb_connect_state(RShandle h, connect_state_cb cb, void* userdata);

/**
 * @fn      fdev_setcb_person
 * @brief   set person related callback function
 * @param[in] h: fdev_init handle
 * @param[in] cbs: callback functions
 * @param[in] userdata: callback function userdata
 * @return  0 OK, other error
 */
RS_API int CALLSPEC fdev_setcb_person(RShandle h, person_cbs cb, void* userdata);

/**
 * @fn      fdev_setcb_recog_mode
 * @brief   set recognition mode related callback function
 * @param[in] h: fdev_init handle
 * @param[in] cb: callback function
 * @param[in] userdata: callback function userdata
 * @return  0 OK, other error
 */
RS_API int CALLSPEC fdev_setcb_recog_mode(RShandle h, set_recog_mode_cb cb, void* userdata);

/**
 * @fn      fdev_setcb_gate_control
 * @brief   set gate control related callback function
 * @param[in] h: fdev_init handle
 * @param[in] cb: callback function
 * @param[in] userdata: callback function userdata
 * @return  0 OK, other error
 */
RS_API int CALLSPEC fdev_setcb_gate_control(RShandle h, gate_control_cb cb, void* userdata);

/**
 * @fn      fdev_setcb_custom_request
 * @brief   set custom request related callback function
 * @param[in] h: fdev_init handle
 * @param[in] cb: callback function
 * @param[in] userdata: callback function userdata
 * @return  0 OK, other error
 */
RS_API int CALLSPEC fdev_setcb_custom_request(RShandle h, custom_request_cb cb, void* userdata);

END_EXTERN_C

#endif // FACE_DEVICE_CLIENT_H_
