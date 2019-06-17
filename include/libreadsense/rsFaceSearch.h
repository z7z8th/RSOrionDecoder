#ifndef RS_FACE_SEARCH_H_
#define RS_FACE_SEARCH_H_

#include "rs_def.h"
#include "rs_face.h"

BEGIN_EXTERN_C

RS_API const char* CALLSPEC rsFaceSearchVersion();
RS_API const char* CALLSPEC rsFaceSearchErrString(int errcode);

RS_API int CALLSPEC rsFaceSearchInit(RShandle* ph, const char* dbfile);
RS_API int CALLSPEC rsFaceSearchCleanup(RShandle* ph);

RS_API int CALLSPEC rsFaceSearchClearFaceDB(RShandle h);
RS_API int CALLSPEC rsFaceSearchGetCount(RShandle h);
RS_API int CALLSPEC rsFaceSearchAddFace(RShandle h, int faceID, rs_face_feature* face_feature);
// Batch delete, more efficient
RS_API int CALLSPEC rsFaceSearchDelFace(RShandle h, int* faceIDs, int num);
RS_API int CALLSPEC rsFaceSearchUpdateFace(RShandle h, int faceID, rs_face_feature* face_feature);
RS_API int CALLSPEC rsFaceSearchGetFace(RShandle h, int faceID, rs_face_feature* face_feature);
RS_API int CALLSPEC rsFaceSearchSearch(RShandle h,
    rs_face_feature* face_feature, int top, float threshold,
    rsFaceSearchResult* results, int* results_num);

END_EXTERN_C

#endif // RS_FACE_SEARCH_H_

