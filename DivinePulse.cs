using UnityEngine;

public class DivinePulse : MonoBehaviour
{
    public float baseIntensity = 1.0f;
    public float pulseFrequency = 0.5f;
    private Material mat;

    void Start()
    {
        mat = GetComponent<Renderer>().material;
    }

    void Update()
    {
        float intensity = baseIntensity + Mathf.Sin(Time.time * pulseFrequency) * 0.5f;
        mat.SetColor("_EmissionColor", Color.green * intensity);
    }
}
