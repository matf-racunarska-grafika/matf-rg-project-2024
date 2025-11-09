//
// Created by matija on 10/30/25.
//

#ifndef MATF_RG_PROJECT_TIMER_HPP
#define MATF_RG_PROJECT_TIMER_HPP


namespace app {

class Timer {
public:
    Timer()=default;
    void restart_timer() {
        m_enabled=false;
        m_time_left=0.0f;
    }
    bool update(float dt) {
        if (m_enabled){
            m_time_left-=dt;
            if(m_time_left<=0) {
                m_enabled = false;
                return false;
            }
            return true;
        }
        return false;
    }
    void start_timer(float time_seconds) {
        m_time_left = time_seconds;
        m_enabled = true;
    }
    bool is_enabled() const { return m_enabled; }
private:
    bool m_enabled=false;
    float m_time_left=0.0f;
};

}// namespace app

#endif//MATF_RG_PROJECT_TIMER_HPP
