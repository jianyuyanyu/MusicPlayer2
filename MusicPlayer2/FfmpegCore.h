#pragma once
#include "IPlayerCore.h"
#include "DllLib.h"
#include "CommonData.h"

#define AV_LOG_ERROR 16
#define AV_LOG_VERBOSE 40
#define AV_LOG_SKIP_REPEATED 1
#define AV_LOG_PRINT_LEVEL 2
typedef struct MusicHandle MusicHandle;
typedef struct MusicInfoHandle MusicInfoHandle;
typedef struct FfmpegCoreSettings FfmpegCoreSettings;
typedef void(*_free_music_handle)(MusicHandle*);
typedef void(*_free_music_info_handle)(MusicInfoHandle*);
typedef void(*_free_ffmpeg_core_settings)(FfmpegCoreSettings*);
typedef int(*_ffmpeg_core_log_format_line)(void* ptr, int level, const char* fmt, va_list vl, char* line, int line_size, int* print_prefix);
typedef void(*_ffmpeg_core_log_set_callback)(void(*callback)(void*, int, const char*, va_list));
typedef void(*_ffmpeg_core_log_set_flags)(int);
typedef int(*_ffmpeg_core_open)(const wchar_t*, MusicHandle**);
typedef int(*_ffmpeg_core_open2)(const wchar_t*, MusicHandle**, FfmpegCoreSettings*);
typedef int(*_ffmpeg_core_info_open)(const wchar_t*, MusicInfoHandle**);
typedef int(*_ffmpeg_core_play)(MusicHandle*);
typedef int(*_ffmpeg_core_pause)(MusicHandle*);
typedef int(*_ffmpeg_core_seek)(MusicHandle*, int64_t);
typedef int(*_ffmpeg_core_set_volume)(MusicHandle*, int);
typedef int(*_ffmpeg_core_set_speed)(MusicHandle*, float);
typedef int(*_ffmpeg_core_get_error)(MusicHandle*);
typedef wchar_t*(*_ffmpeg_core_get_err_msg)(int);
typedef const wchar_t*(*_ffmpeg_core_get_err_msg2)(int);
typedef int64_t(*_ffmpeg_core_get_cur_position)(MusicHandle*);
typedef int(*_ffmpeg_core_song_is_over)(MusicHandle*);
typedef int64_t(*_ffmpeg_core_get_song_length)(MusicHandle*);
typedef int64_t(*_ffmpeg_core_info_get_song_length)(MusicInfoHandle*);
typedef int(*_ffmpeg_core_get_channels)(MusicHandle*);
typedef int(*_ffmpeg_core_info_get_channels)(MusicInfoHandle*);
typedef int(*_ffmpeg_core_get_freq)(MusicHandle*);
typedef int(*_ffmpeg_core_info_get_freq)(MusicInfoHandle*);
typedef int(*_ffmpeg_core_is_playing)(MusicHandle*);
typedef int(*_ffmpeg_core_get_bits)(MusicHandle*);
typedef int(*_ffmpeg_core_info_get_bits)(MusicInfoHandle*);
typedef int64_t(*_ffmpeg_core_get_bitrate)(MusicHandle*);
typedef int64_t(*_ffmpeg_core_info_get_bitrate)(MusicInfoHandle*);
typedef wchar_t*(*_ffmpeg_core_get_metadata)(MusicHandle*, const char* key);
typedef wchar_t*(*_ffmpeg_core_info_get_metadata)(MusicInfoHandle*, const char* key);
typedef int(*_ffmpeg_core_get_fft_data)(MusicHandle*, float*, int);
typedef FfmpegCoreSettings*(*_ffmpeg_core_init_settings)();
typedef int(*_ffmpeg_core_settings_set_volume)(FfmpegCoreSettings*, int volume);
typedef int(*_ffmpeg_core_settings_set_speed)(FfmpegCoreSettings*, float);
typedef int(*_ffmpeg_core_settings_set_cache_length)(FfmpegCoreSettings*, int);
typedef int(*_ffmpeg_core_settings_set_max_retry_count)(FfmpegCoreSettings*, int);

class CFfmpegCore : public IPlayerCore, public CDllLib {
public:
    CFfmpegCore();
    ~CFfmpegCore();
    virtual void InitCore() override;
    virtual void UnInitCore() override;
    virtual unsigned int GetHandle() override;
    virtual std::wstring GetAudioType() override;
    virtual int GetChannels() override;
    virtual int GetFReq() override;
    virtual std::wstring GetSoundFontName() override;
    virtual void Open(const wchar_t* file_path) override;
    virtual void Close() override;
    virtual void Play() override;
    virtual void Pause() override;
    virtual void Stop() override;
    virtual void SetVolume(int volume) override;
    virtual void SetSpeed(float speed) override;
    virtual bool SongIsOver() override;
    virtual int GetCurPosition() override;
    virtual int GetSongLength() override;
    virtual void SetCurPosition(int position) override;
    virtual void GetAudioInfo(SongInfo& song_info, int flag = AF_LENGTH | AF_BITRATE | AF_TAG_INFO) override;
    virtual void GetAudioInfo(const wchar_t* file_path, SongInfo& song_info, int flag = AF_LENGTH | AF_BITRATE | AF_TAG_INFO) override;
    virtual bool IsMidi() override;
    virtual bool IsMidiConnotPlay() override;
    virtual MidiInfo GetMidiInfo() override;
    virtual std::wstring GetMidiInnerLyric() override;
    virtual bool MidiNoLyric() override;
    virtual PlayingState GetPlayingState() override;
    virtual void ApplyEqualizer(int channel, int gain) override;
    virtual void SetReverb(int mix, int time) override;
    virtual void ClearReverb() override;
    virtual void GetFFTData(float fft_data[FFT_SAMPLE]) override;
    virtual int GetErrorCode() override;
    virtual std::wstring GetErrorInfo(int error_code) override;
    virtual std::wstring GetErrorInfo() override;
    virtual PlayerCoreType GetCoreType() override;
    virtual int GetDeviceCount() override;
    std::wstring GetTitle(MusicInfoHandle* h = nullptr);
    std::wstring GetArtist(MusicInfoHandle* h = nullptr);
    std::wstring GetAlbum(MusicInfoHandle* h = nullptr);
    std::wstring GetComment(MusicInfoHandle* h = nullptr);
    std::wstring GetGenre(MusicInfoHandle* h = nullptr);
    std::wstring GetDate(MusicInfoHandle* h = nullptr);
    unsigned short GetYear(MusicInfoHandle* h = nullptr);
    std::wstring GetTrack(MusicInfoHandle* h = nullptr);
    int GetTrackNum(MusicInfoHandle* h = nullptr);
    void UpdateSettings(PlaySettingData* s = nullptr);
    void SetCacheLength(int cache_length = 15);
private:
    std::wstring GetMetadata(std::string key, MusicInfoHandle* h = nullptr);
    virtual bool GetFunction() override;
    static void LogCallback(void*, int, const char*, va_list);
    _free_music_handle free_music_handle = nullptr;
    _free_music_info_handle free_music_info_handle = nullptr;
    _free_ffmpeg_core_settings free_ffmpeg_core_settings = nullptr;
    _ffmpeg_core_log_format_line ffmpeg_core_log_format_line = nullptr;
    _ffmpeg_core_log_set_callback ffmpeg_core_log_set_callback = nullptr;
    _ffmpeg_core_log_set_flags ffmpeg_core_log_set_flags = nullptr;
    _ffmpeg_core_open ffmpeg_core_open = nullptr;
    _ffmpeg_core_open2 ffmpeg_core_open2 = nullptr;
    _ffmpeg_core_info_open ffmpeg_core_info_open = nullptr;
    _ffmpeg_core_play ffmpeg_core_play = nullptr;
    _ffmpeg_core_pause ffmpeg_core_pause = nullptr;
    _ffmpeg_core_seek ffmpeg_core_seek = nullptr;
    _ffmpeg_core_set_volume ffmpeg_core_set_volume = nullptr;
    _ffmpeg_core_set_speed ffmpeg_core_set_speed = nullptr;
    _ffmpeg_core_get_error ffmpeg_core_get_error = nullptr;
    _ffmpeg_core_get_err_msg ffmpeg_core_get_err_msg = nullptr;
    _ffmpeg_core_get_err_msg2 ffmpeg_core_get_err_msg2 = nullptr;
    _ffmpeg_core_get_cur_position ffmpeg_core_get_cur_position = nullptr;
    _ffmpeg_core_song_is_over ffmpeg_core_song_is_over = nullptr;
    _ffmpeg_core_get_song_length ffmpeg_core_get_song_length = nullptr;
    _ffmpeg_core_info_get_song_length ffmpeg_core_info_get_song_length = nullptr;
    _ffmpeg_core_get_channels ffmpeg_core_get_channels = nullptr;
    _ffmpeg_core_info_get_channels ffmpeg_core_info_get_channels = nullptr;
    _ffmpeg_core_get_freq ffmpeg_core_get_freq = nullptr;
    _ffmpeg_core_info_get_freq ffmpeg_core_info_get_freq = nullptr;
    _ffmpeg_core_is_playing ffmpeg_core_is_playing = nullptr;
    _ffmpeg_core_get_bits ffmpeg_core_get_bits = nullptr;
    _ffmpeg_core_info_get_bits ffmpeg_core_info_get_bits = nullptr;
    _ffmpeg_core_get_bitrate ffmpeg_core_get_bitrate = nullptr;
    _ffmpeg_core_info_get_bitrate ffmpeg_core_info_get_bitrate = nullptr;
    _ffmpeg_core_get_metadata ffmpeg_core_get_metadata = nullptr;
    _ffmpeg_core_info_get_metadata ffmpeg_core_info_get_metadata = nullptr;
    _ffmpeg_core_get_fft_data ffmpeg_core_get_fft_data = nullptr;
    _ffmpeg_core_init_settings ffmpeg_core_init_settings = nullptr;
    _ffmpeg_core_settings_set_volume ffmpeg_core_settings_set_volume = nullptr;
    _ffmpeg_core_settings_set_speed ffmpeg_core_settings_set_speed = nullptr;
    _ffmpeg_core_settings_set_cache_length ffmpeg_core_settings_set_cache_length = nullptr;
    _ffmpeg_core_settings_set_max_retry_count ffmpeg_core_settings_set_max_retry_count = nullptr;
    MusicHandle* handle;
    FfmpegCoreSettings* settings = nullptr;
    std::wstring recent_file;
    int err;
};
